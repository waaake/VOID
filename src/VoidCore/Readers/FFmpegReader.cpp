// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "FFmpegReader.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

/* FFmpegDecoder {{{ */
FFmpegDecoder::FFmpegDecoder()
    : m_Path("")
    , m_CurrentFrame(0)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(3)
    , m_FormatContext(nullptr)
    , m_CodecContext(nullptr)
    , m_Frame(nullptr)
    , m_RGBFrame(nullptr)
    , m_Packet(nullptr)
    , m_SwsContext(nullptr)
    , m_Stream(nullptr)
    , m_StreamID(-1)
{
}

FFmpegDecoder::~FFmpegDecoder()
{
    Close();
}

void FFmpegDecoder::Open()
{
    /* Allocate Frames with default values */
    m_Frame = av_frame_alloc();
    m_RGBFrame = av_frame_alloc();
    m_Packet = av_packet_alloc();

    if (avformat_open_input(&m_FormatContext, m_Path.c_str(), nullptr, nullptr) > 0)
        return;

    if (avformat_find_stream_info(m_FormatContext, nullptr) < 0)
        return;

    m_StreamID = av_find_best_stream(m_FormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    m_CurrentFrame = 0;
    if (m_StreamID < 0)
        return;

    m_Stream = m_FormatContext->streams[m_StreamID];

    AVCodecParameters* codecParams = m_Stream->codecpar;

    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    m_CodecContext = avcodec_alloc_context3(codec);

    if (m_CodecContext->pix_fmt == AV_PIX_FMT_RGBA)
        m_Channels = 4;

    /* Update the codec context based on the values from the codec params */
    avcodec_parameters_to_context(m_CodecContext, codecParams);
    avcodec_open2(m_CodecContext, codec, nullptr);

    /* Update the resolution information */
    m_Width = m_CodecContext->width;
    m_Height = m_CodecContext->height;
}

void FFmpegDecoder::Close()
{
    if (m_Frame) av_frame_free(&m_Frame);
    if (m_RGBFrame) av_frame_free(&m_RGBFrame);
    if (m_Packet) av_packet_free(&m_Packet);
    if (m_CodecContext) avcodec_free_context(&m_CodecContext);
    if (m_FormatContext) avformat_close_input(&m_FormatContext);
    if (m_SwsContext) sws_free_context(&m_SwsContext);

    /* The read stream ID */
    m_StreamID = -1;
}

bool FFmpegDecoder::Decode(const std::string& path, const int framenumber, std::vector<unsigned char>& pixels)
{
    /**
     * At the moment, this is not accessible concurrently throught multiple threads
     * Rather than handling this specifically at the cache level, using a guard here
     * TODO: Maybe handling all movies to be single threaded can help solve this in a better way
     * still this shouldn't cause any issues...
     */
    std::lock_guard<std::mutex> guard(m_Mutex);

    /* A new movie is being read */
    if (path != m_Path)
    {
        Close();
        m_Path = path;
        Open();
    }

    pixels.resize(av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_Width, m_Height, 1));

    /* Setup data pointers */
    av_image_fill_arrays(m_RGBFrame->data, m_RGBFrame->linesize, pixels.data(), AV_PIX_FMT_RGB24, m_Width, m_Height, 1);

    /**
     * Allocate the sws context
     * not scaling the image, have set the destination width and height to be the same as the source for now
     */
    m_SwsContext = sws_getContext(m_Width, m_Height, m_CodecContext->pix_fmt, m_Width, m_Height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

    /* Now we start */
    bool found = false;
    int retryCount = 0;

    int distance = 0;
    bool seeked = false;

    /* Retry seeking for 3 times before giving up */
    while (!found && retryCount < 3)
    {
        /**
         * Calculate the distance between the requested and the last frame which was read 
         * this helps us determine whether or not to save any data and also if we need to seek forwards in order
         * to reach the frame quickly
         * Always seeking isn't helpful, so seeking can be done if the distance is greater than 20 frames
         * and any frames from 10 frames to the requested can be saved in case they are needed in the next intermediate
         */
        distance = framenumber - m_CurrentFrame;
        /* Decode the next frame and it returns back either a negative value or the decoded frame */
        v_frame_t ret = DecodeNextFrame((distance < 10));

        /**
         * Then we check if the return value was greater than the requested frame
         * if so, we might need to seek back and try again (max 3 times)
         * 
         * Meanwhile also check if the file was at the end of it's frame? and we want to read it again?
         */
        if (ret > framenumber || ret == -1)
        {
            /* Seek */
            int64_t seek_pts = av_rescale_q(framenumber, av_inv_q(m_Stream->r_frame_rate), m_Stream->time_base);
            /* Seek the closest keyframe before the framenumber */
            av_seek_frame(m_FormatContext, m_StreamID, seek_pts, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(m_CodecContext);

            /* Increment the retry count */
            retryCount++;
        }
        else if (ret == framenumber)
        {
            /* We found the frame */
            found = true;
            break;
        }
        else if (distance > 20 && !seeked)
        {
            /* Seek */
            int64_t seek_pts = av_rescale_q(framenumber, av_inv_q(m_Stream->r_frame_rate), m_Stream->time_base);
            /**
             * Seek the framenumber
             * using AVSEEK_FLAG_FRAME to seek to any frame and don't really care about keyframes
             */
            av_seek_frame(m_FormatContext, m_StreamID, seek_pts, AVSEEK_FLAG_FRAME);
            avcodec_flush_buffers(m_CodecContext);

            seeked = true;
        }
    }

    return found;
}

v_frame_t FFmpegDecoder::DecodeNextFrame(bool save)
{
    /* Read the next frame from the packet */
    if (av_read_frame(m_FormatContext, m_Packet) < 0)
        return -1;

    if (m_Packet->stream_index != m_StreamID)
        return -2;

    avcodec_send_packet(m_CodecContext, m_Packet);
    if (avcodec_receive_frame(m_CodecContext, m_Frame) != 0)
        return -3;

    m_CurrentFrame = av_rescale_q(m_Frame->pts, m_Stream->time_base, av_inv_q(m_Stream->r_frame_rate));
    if (save)
        sws_scale(m_SwsContext, m_Frame->data, m_Frame->linesize, 0, m_Height, m_RGBFrame->data, m_RGBFrame->linesize);

    av_packet_unref(m_Packet);

    /* The decoded frame number*/
    return m_CurrentFrame;
}

/* }}} */

/* FFmpegPixReader {{{ */
FFmpegPixReader::FFmpegPixReader(const std::string& path, v_frame_t framenumber)
    : VoidMPixReader(path, framenumber)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
    , m_AChannels(0)
    , m_Samplerate(0)
    , m_Startframe(0)
    , m_Endframe(0)
    , m_Duration(0)
    , m_Framerate(0.0)
{
}

FFmpegPixReader::~FFmpegPixReader()
{
    Clear();
}

void FFmpegPixReader::Clear()
{
    /* Remove any data from the pixels vector and shrink it back in place */
    m_Pixels.clear();
    m_Pixels.shrink_to_fit();
}

void FFmpegPixReader::ProcessInformation()
{
    /* Isn't anything to process */
    if (m_Path.empty())
        return;

    AVFormatContext* formatContext = nullptr;
    if (avformat_open_input(&formatContext, m_Path.c_str(), nullptr, nullptr) >= 0)
    {
        for (unsigned int i = 0; i < formatContext->nb_streams; ++i)
        {
            const AVStream* stream = formatContext->streams[i];
            if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                /* This always has been 2 less than the total number of frames, need a bit more information here... */
                m_Duration = stream->nb_frames - 2;
                m_Framerate = av_q2d(stream->r_frame_rate);
                m_Endframe = m_Duration - 1;
            }
            else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                m_Samplerate = stream->codecpar->sample_rate;
                m_AChannels = stream->codecpar->ch_layout.nb_channels;
            }
        }

        avformat_close_input(&formatContext);
    }
}

MFrameRange FFmpegPixReader::Framerange()
{
    /* If the duration is 0 that means the information wasn't processed yet */
    if (!m_Duration)
        ProcessInformation();

    return {m_Startframe, m_Endframe, m_Duration};
}

double FFmpegPixReader::Framerate()
{
    if (!m_Framerate)
        ProcessInformation();

    return m_Framerate;
}

void FFmpegPixReader::Read()
{
    /* Decoder */
    FFmpegDecoder& decoder = FFmpegDecoder::Instance();
    if (decoder.Decode(m_Path, m_Framenumber, m_Pixels))
    {
        /* Read the Frame Dimensions */
        m_Width = decoder.Width();
        m_Height = decoder.Height();

        m_Channels = decoder.Channels();
    }
}

const std::map<std::string, std::string> FFmpegPixReader::Metadata() const
{
    std::map<std::string, std::string> m;

    AVFormatContext* formatContext = nullptr;

    if (avformat_open_input(&formatContext, m_Path.c_str(), nullptr, nullptr) >= 0)
    {
        /* Get Stream info */
        if (avformat_find_stream_info(formatContext, nullptr) < 0)
        {
            avformat_close_input(&formatContext);
            return m;
        }

        AVStream* vidStream = nullptr;
        int streamId = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

        /* Not found */
        if (streamId < 0)
        {
            avformat_close_input(&formatContext);
            return m;
        }

        /* Iterate over all metadata for the file */
        AVDictionaryEntry* tag = nullptr;
        /* Match the starting component of the key to get the find the tag --> None will match every */
        while((tag = av_dict_get(formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
        {
            m[tag->key] = tag->value;
        }

        vidStream = formatContext->streams[streamId];

        /* This always has been 2 less than the total number of frames, need a bit more information here... */
        int endframe = vidStream->nb_frames - 2;
        double framerate = av_q2d(vidStream->r_frame_rate);
        m["end_frame"] = std::to_string(endframe);
        m["duration"] = std::to_string(endframe - m_Startframe + 1);
        m["framerate"] = std::to_string(framerate);

        /* Deallocate internals */
        avformat_close_input(&formatContext);
    }

    /* Basic Metadata */
    m["filepath"] = m_Path;
    m["width"] = std::to_string(m_Height);
    m["height"] = std::to_string(m_Width);
    m["channels"] = std::to_string(m_Channels);
    m["start_frame"] = std::to_string(m_Startframe);

    return m;
}

/* }}} */

VOID_NAMESPACE_CLOSE
