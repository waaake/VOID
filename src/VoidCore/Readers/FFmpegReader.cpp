// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "FFmpegReader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* FFmpegDecoder {{{ */
FFmpegDecoder::FFmpegDecoder()
    : m_Width(0)
    , m_Height(0)
    , m_Channels(3)
    , m_VStreamID(-1)
    , m_AStreamID(-1)
    , m_FormatContext(nullptr)
    , m_VCodecContext(nullptr)
    , m_ACodecContext(nullptr)
    , m_VFrame(nullptr)
    , m_RGBFrame(nullptr)
    , m_AFrame(nullptr)
    , m_Packet(nullptr)
    , m_SwsContext(nullptr)
    , m_SwrContext(nullptr)
    , m_VStream(nullptr)
    , m_AStream(nullptr)
    , m_Path("")
    , m_CurrentFrame(0)
{
}

FFmpegDecoder::~FFmpegDecoder()
{
    Close();
}

void FFmpegDecoder::Open()
{
    m_VFrame = av_frame_alloc();
    m_AFrame = av_frame_alloc();
    m_RGBFrame = av_frame_alloc();
    m_Packet = av_packet_alloc();

    m_CurrentFrame = 0;

    if (avformat_open_input(&m_FormatContext, m_Path.c_str(), nullptr, nullptr) > 0)
        return;

    if (avformat_find_stream_info(m_FormatContext, nullptr) < 0)
        return;

    m_VStreamID = av_find_best_stream(m_FormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    m_AStreamID = av_find_best_stream(m_FormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

    if (m_VStreamID < 0)
        return;

    m_VStream = m_FormatContext->streams[m_VStreamID];

    if (m_AStreamID >= 0)
    {
        m_AStream = m_FormatContext->streams[m_AStreamID];
        const AVCodecParameters* codecParams = m_AStream->codecpar;
        const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);

        m_ACodecContext = avcodec_alloc_context3(codec);

        avcodec_parameters_to_context(m_ACodecContext, codecParams);
        avcodec_open2(m_ACodecContext, codec, nullptr);
        m_SwrContext = swr_alloc();

        AVChannelLayout layout;
        av_channel_layout_default(&layout, m_ACodecContext->ch_layout.nb_channels);
        swr_alloc_set_opts2(&m_SwrContext, &layout, AV_SAMPLE_FMT_S16, m_ACodecContext->sample_rate, &layout, m_ACodecContext->sample_fmt, m_ACodecContext->sample_rate, 0, nullptr);
        swr_init(m_SwrContext);
    }

    const AVCodecParameters* codecParams = m_VStream->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);

    m_VCodecContext = avcodec_alloc_context3(codec);
    if (m_VCodecContext->pix_fmt == AV_PIX_FMT_RGBA)
        m_Channels = 4;

    avcodec_parameters_to_context(m_VCodecContext, codecParams);
    avcodec_open2(m_VCodecContext, codec, nullptr);

    /* Update the resolution information */
    m_Width = m_VCodecContext->width;
    m_Height = m_VCodecContext->height;
}

void FFmpegDecoder::Close()
{
    if (m_VFrame) av_frame_free(&m_VFrame);
    if (m_RGBFrame) av_frame_free(&m_RGBFrame);
    if (m_AFrame) av_frame_free(&m_AFrame);
    if (m_Packet) av_packet_free(&m_Packet);
    if (m_VCodecContext) avcodec_free_context(&m_VCodecContext);
    if (m_ACodecContext) avcodec_free_context(&m_ACodecContext);
    if (m_FormatContext) avformat_close_input(&m_FormatContext);
    if (m_SwsContext) sws_free_context(&m_SwsContext);
    if (m_SwrContext) swr_free(&m_SwrContext);

    m_DecodedFrames.clear();
    m_DecodedStreams.clear();
    m_VStreamID = -1;
}

std::vector<unsigned char>& FFmpegDecoder::VideoFrame(const int framenumber)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return VideoData(framenumber);
}

std::vector<unsigned char>& FFmpegDecoder::Audio(const int framenumber)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return AudioStream(framenumber);
}

std::vector<unsigned char>& FFmpegDecoder::VideoData(const int frame)
{
    auto it = m_DecodedFrames.find(frame);
    if (it == m_DecodedFrames.end())
    {
        m_DecodedFrames[frame] = {};
        return m_DecodedFrames[frame];
    }
    return it->second;
}

std::vector<unsigned char>& FFmpegDecoder::AudioStream(const int framenumber)
{
    auto it = m_DecodedStreams.find(framenumber);
    if (it == m_DecodedStreams.end())
    {
        m_DecodedStreams[framenumber] = {};
        return m_DecodedStreams[framenumber];
    }
    return it->second;
}

void FFmpegDecoder::DecodeVideo(const std::string& path, const int framenumber)
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

    std::vector<unsigned char>& pixels = VideoData(framenumber);
    if (!pixels.empty())
        return;

    pixels.resize(av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_Width, m_Height, 1));
    av_image_fill_arrays(m_RGBFrame->data, m_RGBFrame->linesize, pixels.data(), AV_PIX_FMT_RGB24, m_Width, m_Height, 1);

    /**
     * Allocate the sws context
     * not scaling the image, have set the destination width and height to be the same as the source for now
     */
    m_SwsContext = sws_getContext(m_Width, m_Height, m_VCodecContext->pix_fmt, m_Width, m_Height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

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
            avcodec_flush_buffers(m_VCodecContext);

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
            avcodec_flush_buffers(m_VCodecContext);

            seeked = true;
        }
    }
}

void FFmpegDecoder::DecodeAudio(const int framenumber)
{
    if (m_AStreamID < 0)
        return;

    /**
     * Case 1: The current framenumber being looked at is higher than the current framenumber
     * this means the audio at the frame must have been decoded for sure
     * 20 is the min
     */
    if ((m_CurrentFrame - 20) > framenumber)
        return;


    /**
     * Case 2: This was decoded before and already available for use
     */
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (!AudioStream(framenumber).empty())
            return;
    }

    /**
     * Last case when the audio hasn't been decoded yet
     * To make sure we decode the audio correctly, Decode the next video frame
     * (which mostly decodes the current and next frame)
     */
    DecodeVideo(m_Path, framenumber + 1);
}

v_frame_t FFmpegDecoder::DecodeNextFrame(bool save)
{
    if (av_read_frame(m_FormatContext, m_Packet) < 0)
        return -1;

    if (m_Packet->stream_index == m_AStreamID)
        DecodeNextAudio();

    if (m_Packet->stream_index == m_VStreamID)
    {
        /* Send the packet */
        avcodec_send_packet(m_VCodecContext, m_Packet);
        if (avcodec_receive_frame(m_VCodecContext, m_VFrame) != 0)
            return -3;

        m_CurrentFrame = av_rescale_q(m_VFrame->pts, m_VStream->time_base, av_inv_q(m_VStream->r_frame_rate));
        if (save)
            sws_scale(m_SwsContext, m_VFrame->data, m_VFrame->linesize, 0, m_Height, m_RGBFrame->data, m_RGBFrame->linesize);
    }
    else
    {
        /* Dereference the buffer */
        // av_packet_unref(m_Packet);
        return -2;
    }

    /* Dereference the buffer */
    av_packet_unref(m_Packet);
    return m_CurrentFrame;
}

void FFmpegDecoder::DecodeNextAudio()
{
    avcodec_send_packet(m_ACodecContext, m_Packet);
    if (avcodec_receive_frame(m_ACodecContext, m_AFrame) == 0)
    {
        int outLines;
        int outSamples = av_rescale_rnd(m_AFrame->nb_samples, m_ACodecContext->sample_rate, m_ACodecContext->sample_rate, AV_ROUND_UP);
        int outBufferSize = av_samples_get_buffer_size(&outLines, m_ACodecContext->ch_layout.nb_channels, m_AFrame->nb_samples, AV_SAMPLE_FMT_S16, 1);

        std::vector<unsigned char>& stream = AudioStream(m_CurrentFrame);
        stream.resize(outBufferSize);
        unsigned char* strmptr = stream.data();
        swr_convert(m_SwrContext, (uint8_t**)&strmptr, outSamples, (const uint8_t**)(m_AFrame->extended_data), m_AFrame->nb_samples);
    }
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

    // m_Stream.clear();
    // m_Stream.shrink_to_fit();
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
                m_Endframe = stream->nb_frames - 2;
                m_Framerate = av_q2d(stream->r_frame_rate);
            }
            else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                avformat_find_stream_info(formatContext, nullptr);  // This is expensive, check if there is a different way to get audio channel count
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
    decoder.DecodeVideo(m_Path, m_Framenumber);

    std::swap(decoder.VideoFrame(m_Framenumber), m_Pixels);

    decoder.DecodeAudio(m_Framenumber);
    std::swap(decoder.Audio(m_Framenumber), m_Stream);
    // VOID_LOG_INFO("Frame: {0}, Empty Audio: {1}", m_Framenumber, m_Stream.empty());

    /* Read the Frame Dimensions */
    m_Width = decoder.Width();
    m_Height = decoder.Height();
    m_Channels = decoder.Channels();
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
