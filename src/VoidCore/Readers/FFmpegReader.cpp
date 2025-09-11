// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "FFmpegReader.h"
#include "VoidCore/Logging.h"

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

    /* Get the ID to the best stream of the Movie file */
    m_StreamID = av_find_best_stream(m_FormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    /* Reset the current frame */
    m_CurrentFrame = 0;

    if (m_StreamID < 0)
        return;

    /* Video stream for the StreamID */
    m_Stream = m_FormatContext->streams[m_StreamID];

    /* Codec Related Parameters */
    AVCodecParameters* codecParams = m_Stream->codecpar;

    /* Find a decoder for the codec */
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);

    /* Initialize defaults for the given codec */
    m_CodecContext = avcodec_alloc_context3(codec);

    /* Update the channels if we're RGBA */
    if (m_CodecContext->pix_fmt == AV_PIX_FMT_RGBA)
        m_Channels = 4;

    /* Update the codec context based on the values from the codec params */
    avcodec_parameters_to_context(m_CodecContext, codecParams);

    /* Initialize the context to use the given codec */
    avcodec_open2(m_CodecContext, codec, nullptr);

    /* Update the resolution information */
    m_Width = m_CodecContext->width;
    m_Height = m_CodecContext->height;
}

void FFmpegDecoder::Close()
{
    if (m_Frame)
        av_frame_free(&m_Frame);

    if (m_RGBFrame)
        av_frame_free(&m_RGBFrame);

    if (m_Packet)
        av_packet_free(&m_Packet);

    if (m_CodecContext)
        avcodec_free_context(&m_CodecContext);

    if (m_FormatContext)
        avformat_close_input(&m_FormatContext);

    if (m_SwsContext)
        sws_free_context(&m_SwsContext);

    /* Reset caches */
    m_Pixels.clear();
    m_Pixels.shrink_to_fit();
    m_DecodedFrames.clear();

    /* The read stream ID */
    m_StreamID = -1;
}

std::vector<unsigned char>& FFmpegDecoder::GetVector(const int frame)
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
    if (!GetVector(framenumber).empty())
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
}

v_frame_t FFmpegDecoder::DecodeNextFrame(bool save)
{
    /* Read the next frame from the packet */
    int status = av_read_frame(m_FormatContext, m_Packet);

    /* Indicates a Read failure */
    if (status < 0)
        return -1;

    /* Indicates just a stream mismatch and can continue back */
    if (m_Packet->stream_index != m_StreamID)
        return -2;

    /* Send the packet */
    avcodec_send_packet(m_CodecContext, m_Packet);
    /* Recieve back the frame */
    status = avcodec_receive_frame(m_CodecContext, m_Frame);

    /* Check if we can proceed further */
    if (status != 0)
        return -3;

    m_CurrentFrame = av_rescale_q(m_Frame->pts, m_Stream->time_base, av_inv_q(m_Stream->r_frame_rate));

    if (save)
    {
        sws_scale(m_SwsContext, m_Frame->data, m_Frame->linesize, 0, m_Height, m_RGBFrame->data, m_RGBFrame->linesize);

        std::vector<unsigned char>& v = GetVector(m_CurrentFrame);
        v = m_Pixels;
    }

    /* Dereference the buffer */
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
                vidStream = formatContext->streams[i]; break;
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
    std::swap(decoder.GetData(m_Framenumber), m_Pixels);

    /* Read the Frame Dimensions */
    m_Width = decoder.Width();
    m_Height = decoder.Height();

    m_Channels = decoder.Channels();
}

/* }}} */

VOID_NAMESPACE_CLOSE
