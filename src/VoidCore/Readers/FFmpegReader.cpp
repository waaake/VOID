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
    , m_VCodecCtx(nullptr)
    , m_Frame(nullptr)
    , m_RGBFrame(nullptr)
    , m_Packet(nullptr)
    , m_SwsContext(nullptr)
    , m_VStream(nullptr)
    , m_VStreamID(-1)
{
}

void FFmpegDecoder::Open()
{
    /* Allocate Frames with default values */
    m_Frame = av_frame_alloc();
    m_RGBFrame = av_frame_alloc();

    /* Allocate AVPacket */
    m_Packet = av_packet_alloc();

    /* Try opening the file */
    if (avformat_open_input(&m_FormatContext, m_Path.c_str(), nullptr, nullptr) > 0)
        return;

    /* Try finding stream information */
    if (avformat_find_stream_info(m_FormatContext, nullptr) < 0)
        return;

    /* Reset the current frame */
    m_CurrentFrame = 0;

    InitContext();
}

void FFmpegDecoder::InitContext()
{
    /* Get the ID to the best stream of the Movie file */
    m_VStreamID = av_find_best_stream(m_FormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    if (m_VStreamID < 0)
        return;

    m_VStream = m_FormatContext->streams[m_VStreamID];

    AVCodecParameters* codecParams = m_VStream->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);

    m_VCodecCtx = avcodec_alloc_context3(codec);

    /* Update the channels if we're RGBA */
    if (m_VCodecCtx->pix_fmt == AV_PIX_FMT_RGBA)
        m_Channels = 4;

    avcodec_parameters_to_context(m_VCodecCtx, codecParams);
    avcodec_open2(m_VCodecCtx, codec, nullptr);

    /* Update the resolution information */
    m_Width = m_VCodecCtx->width;
    m_Height = m_VCodecCtx->height;
}

void FFmpegDecoder::Close()
{
    if (m_Frame)
        av_frame_free(&m_Frame);

    if (m_RGBFrame)
        av_frame_free(&m_RGBFrame);

    if (m_Packet)
        av_packet_free(&m_Packet);

    if (m_VCodecCtx)
        avcodec_free_context(&m_VCodecCtx);

    if (m_FormatContext)
        avformat_close_input(&m_FormatContext);

    if (m_SwsContext)
        sws_free_context(&m_SwsContext);

    /* Reset caches */
    m_Pixels.clear();
    m_Pixels.shrink_to_fit();
    m_DecodedFrames.clear();

    /* The read stream ID */
    m_VStreamID = -1;
}

std::vector<unsigned char>& FFmpegDecoder::Frame(const int framenumber)
{
    std::lock_guard<std::mutex> guard(m_Mutex);
    return FrameVector(framenumber);
}

std::vector<unsigned char>& FFmpegDecoder::FrameVector(const int frame)
{
    if (m_DecodedFrames.find(frame) == m_DecodedFrames.end())
    {
        /* Assign a new vector to the decoded frames */
        m_DecodedFrames[frame] = {};
        return m_DecodedFrames.at(frame);
    }

    return m_DecodedFrames.at(frame);
}

void FFmpegDecoder::Decode(const std::string& path, const int framenumber)
{
    /**
     * At the moment, this is not accessible concurrently throught multiple threads
     * Rather than handling this specifically at the cache level, using a guard here
     * TODO: Maybe handling all movies to be single threaded can help solve this in a better way
     * still this shouldn't cause any issues...
     */
    std::lock_guard<std::mutex> guard(m_Mutex);

    /* The framenumber was already decoded as stored */
    if (!FrameVector(framenumber).empty())
    {
        return;
    }

    /* A new movie is being read */
    if (path != m_Path)
    {
        /* Close existing file and buffers */
        Close();

        /* Update the internal path */
        m_Path = path;

        /* And open the file */
        Open();
    }

     /* Get how much size is required to read the image */
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_Width, m_Height, 1);
    m_Pixels.resize(numBytes);

    /* Setup data pointers */
    av_image_fill_arrays(m_RGBFrame->data, m_RGBFrame->linesize, m_Pixels.data(), AV_PIX_FMT_RGB24, m_Width, m_Height, 1);

    /**
     * Allocate the sws context
     * not scaling the image, have set the destination width and height to be the same as the source for now
     */
    m_SwsContext = sws_getContext(m_Width, m_Height, m_VCodecCtx->pix_fmt, m_Width, m_Height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

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
            int64_t seek_pts = av_rescale_q(framenumber, av_inv_q(m_VStream->r_frame_rate), m_VStream->time_base);
            /* Seek the closest keyframe before the framenumber */
            av_seek_frame(m_FormatContext, m_VStreamID, seek_pts, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(m_VCodecCtx);

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
            int64_t seek_pts = av_rescale_q(framenumber, av_inv_q(m_VStream->r_frame_rate), m_VStream->time_base);
            /**
             * Seek the framenumber
             * using AVSEEK_FLAG_FRAME to seek to any frame and don't really care about keyframes
             */
            av_seek_frame(m_FormatContext, m_VStreamID, seek_pts, AVSEEK_FLAG_FRAME);
            avcodec_flush_buffers(m_VCodecCtx);

            seeked = true;
        }
    }
}

v_frame_t FFmpegDecoder::DecodeNextFrame(bool save)
{
    /* Read the next frame from the packet */
    int status = av_read_frame(m_FormatContext, m_Packet);

    /* Indicates a Read failure */
    if (status < 0)
        return -1;

    /* Indicates just a stream mismatch and can continue back */
    if (m_Packet->stream_index != m_VStreamID)
        return -2;

    status = DecodeVideo(save);

    if (status < 0)
        return status;

    /* Dereference the buffer */
    av_packet_unref(m_Packet);

    /* The decoded frame number*/
    return m_CurrentFrame;
}

v_frame_t FFmpegDecoder::DecodeVideo(bool save)
{
    /* Send the packet */
    avcodec_send_packet(m_VCodecCtx, m_Packet);
    /* Recieve back the frame */
    if (avcodec_receive_frame(m_VCodecCtx, m_Frame) != 0)
        return -3;

    // if (status != 0)
    //     return -3;

    m_CurrentFrame = av_rescale_q(m_Frame->pts, m_VStream->time_base, av_inv_q(m_VStream->r_frame_rate));

    if (save)
    {
        sws_scale(m_SwsContext, m_Frame->data, m_Frame->linesize, 0, m_Height, m_RGBFrame->data, m_RGBFrame->linesize);

        std::vector<unsigned char>& v = FrameVector(m_CurrentFrame);
        v = m_Pixels;
    }

    return m_CurrentFrame;
}

/* }}} */

/* FFmpegPixReader {{{ */
FFmpegPixReader::FFmpegPixReader(const std::string& path, v_frame_t framenumber)
    : VoidMPixReader(path, framenumber)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
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
        AVStream* vidStream = nullptr;
        for (unsigned int i = 0; i < formatContext->nb_streams; ++i)
        {
            if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                vidStream = formatContext->streams[i];
                break;
            }
        }

        /* This always has been 2 less than the total number of frames, need a bit more information here... */
        m_Endframe = vidStream->nb_frames - 2;

        AVRational framerate = vidStream->r_frame_rate;
        m_Framerate = av_q2d(framerate);

        /* Deallocate internals */
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

    decoder.Decode(m_Path, m_Framenumber);

    /**
     * Once the decoding for the frame is completed
     * Swap the needed frame data with empty undelying pixel struct
     */
    std::swap(decoder.Frame(m_Framenumber), m_Pixels);
    // std::swap(decoder.AudioData(m_Framenumber), m_AudioSample);

    // VOID_LOG_INFO("Audio Data Empty: {0}", m_AudioSample.size());

    /* Read the Frame Dimensions */
    m_Width = decoder.Width();
    m_Height = decoder.Height();

    m_Channels = decoder.Channels();
}

AudioBuffer FFmpegPixReader::Audio() const
{
    AudioBuffer buffer;

    AVFormatContext* fmtCtx = nullptr;
    if (avformat_open_input(&fmtCtx, m_Path.c_str(), nullptr, nullptr) < 0)
        return buffer;
    
    if (avformat_find_stream_info(fmtCtx, nullptr) < 0)
        return buffer;
    
    int index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

    if (index < 0)
        return buffer;
    
    AVCodecParameters* codecParams = fmtCtx->streams[index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    AVCodecContext* codecCtx = avcodec_alloc_context3(codec);

    avcodec_parameters_to_context(codecCtx, codecParams);
    avcodec_open2(codecCtx, codec, nullptr);

    SwrContext* swrCtx = swr_alloc();

    AVChannelLayout chLayout;
    av_channel_layout_default(&chLayout, codecCtx->ch_layout.nb_channels);

    swr_alloc_set_opts2(&swrCtx, &chLayout, AV_SAMPLE_FMT_S16, codecCtx->sample_rate, &chLayout, codecCtx->sample_fmt, codecCtx->sample_rate, 0, nullptr);
    swr_init(swrCtx);

    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    while (av_read_frame(fmtCtx, packet) >= 0)
    {
        if (packet->stream_index == index)
        {
            if (avcodec_send_packet(codecCtx, packet) == 0)
            {
                while (avcodec_receive_frame(codecCtx, frame) == 0)
                {
                    int outlines;
                    int outsamples = av_rescale_rnd(frame->nb_samples, codecCtx->sample_rate, codecCtx->sample_rate, AV_ROUND_UP);

                    int bufsize = av_samples_get_buffer_size(&outlines, 2, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);

                    std::vector<unsigned char> temp(bufsize);
                    unsigned char* data = temp.data();

                    swr_convert(swrCtx, &data, outsamples, (const uint8_t**)frame->extended_data, frame->nb_samples);

                    buffer.data.insert(buffer.data.end(), temp.begin(), temp.end());
                }
            }
        }
        av_packet_unref(packet);
    }

    /* Additional Details */
    buffer.channels = codecCtx->ch_layout.nb_channels;
    buffer.samplerate = codecCtx->sample_rate;
    buffer.format = AudioFormat::S_16;
    buffer.pts = 0;

    /* Free */
    swr_free(&swrCtx);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecCtx);
    avformat_close_input(&fmtCtx);

    return buffer;
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
