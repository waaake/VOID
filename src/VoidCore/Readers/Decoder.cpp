// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>
#include <memory>

/* TBB */
#include <tbb/parallel_for.h>

/* Internal */
#include "FFmpegReader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

// This governs how many parallel instances of the Decoder can exist at ones
// allowing multiple streams of videos to be decoded for playback
// we may want to allow configuring this later on depending on system specs
// For now we allow 16 as something to test as well
static constexpr std::size_t MAX_DECODERS = 16;

/// FFmpegDecoder

FFmpegDecoder::FFmpegDecoder()
    : m_Path("")
    , m_CurrentFrame(0)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(3)
    , m_Format(nullptr)
    , m_CodecContext(nullptr)
    , m_Frame(nullptr)
    , m_RGBFrame(nullptr)
    , m_Packet(nullptr)
    , m_Scaler(nullptr)
    , m_Stream(nullptr)
    , m_VideoStreamIndex(-1)
{
}

FFmpegDecoder::~FFmpegDecoder()
{
    Close();
}

FFmpegDecoder& FFmpegDecoder::Instance(const std::string& path)
{
    static std::unordered_map<std::string, std::unique_ptr<FFmpegDecoder>> s_decoders;
    static std::size_t s_head = 0;

    auto it = s_decoders.find(path);
    if (it == s_decoders.end())
    {
        std::unique_ptr<FFmpegDecoder> decoder = nullptr;
        if (s_decoders.size() == MAX_DECODERS)
        {
            auto iter = s_decoders.begin();
            std::advance(iter, s_head);
            decoder = std::move(iter->second);
            s_decoders.erase(iter);
        }

        s_decoders[path] = decoder == nullptr ? std::make_unique<FFmpegDecoder>() : std::move(decoder);
        s_head = (s_head + 1) % MAX_DECODERS;
    }

    return *(s_decoders[path].get());
}

void FFmpegDecoder::Open()
{
    m_Frame = av_frame_alloc();
    m_RGBFrame = av_frame_alloc();
    m_Packet = av_packet_alloc();

    if (avformat_open_input(&m_Format, m_Path.c_str(), nullptr, nullptr) > 0)
        return;

    if (avformat_find_stream_info(m_Format, nullptr) < 0)
        return;

    m_VideoStreamIndex = av_find_best_stream(m_Format, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    m_CurrentFrame = 0;
    if (m_VideoStreamIndex < 0) return;

    m_Stream = m_Format->streams[m_VideoStreamIndex];
    AVCodecParameters* codecParams = m_Stream->codecpar;

    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    m_CodecContext = avcodec_alloc_context3(codec);

    if (m_CodecContext->pix_fmt == AV_PIX_FMT_RGBA)
        m_Channels = 4;

    avcodec_parameters_to_context(m_CodecContext, codecParams);
    avcodec_open2(m_CodecContext, codec, nullptr);

    m_Width = m_CodecContext->width;
    m_Height = m_CodecContext->height;

    m_Scaler = sws_getCachedContext(
        m_Scaler,
        m_Width,
        m_Height,
        m_CodecContext->pix_fmt,
        m_Width,
        m_Height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        nullptr,
        nullptr,
        nullptr
    );

    // av_dump_format(m_Format, 0, m_Path.c_str(), 0);
}

void FFmpegDecoder::Close()
{
    if (m_Frame) av_frame_free(&m_Frame);
    if (m_RGBFrame) av_frame_free(&m_RGBFrame);
    if (m_Packet) av_packet_free(&m_Packet);
    if (m_CodecContext) avcodec_free_context(&m_CodecContext);
    if (m_Format) avformat_close_input(&m_Format);

    #if LIBSWSCALE_VERSION_MAJOR < 9
    if (m_Scaler) sws_freeContext(m_Scaler);
    m_Scaler = nullptr;
    #else
    if (m_Scaler) sws_free_context(&m_Scaler);
    #endif

    m_VideoStreamIndex = -1;
    m_Buffer.Clear();
}

bool FFmpegDecoder::Decode(const std::string& path, const int framenumber, Buffer<float>& pixels)
{
    /**
     * At the moment, this is not accessible concurrently throught multiple threads
     * Rather than handling this specifically at the cache level, using a guard here
     * TODO: Maybe handling all movies to be single threaded can help solve this in a better way
     * still this shouldn't cause any issues...
     */
    std::lock_guard<std::mutex> guard(m_Mutex);

    // A new movie is being read
    if (path != m_Path)
    {
        Close();
        m_Path = path;
        Open();
    }

    if (m_Buffer.Empty())
    {
        m_Buffer.Resize(av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_Width, m_Height, 1));
        av_image_fill_arrays(m_RGBFrame->data, m_RGBFrame->linesize, m_Buffer.Data(), AV_PIX_FMT_RGB24, m_Width, m_Height, 1);
    }

    pixels._buf.resize(m_Buffer._buf.size());

    bool found = false;
    int retryCount = 0;
    bool seeked = false;
    bool flushed = false;

    // Retry seeking for 3 times before giving up
    while (!found && retryCount < 3)
    {
        // Decode the next frame and it returns back either a negative value or the decoded frame
        v_frame_t ret = DecodeNextFrame();
        VOID_LOG_INFO("Ret: {}", ret);

        /**
         * Then we check if the return value was greater than the requested frame
         * if so, we might need to seek back and try again (max 3 times)
         *
         * Meanwhile also check if the file was at the end of it's frame? and we want to read it again?
         */
        if (ret == -1 && !flushed)
        {
            DecodePacket(m_CodecContext, nullptr);
            flushed = true;
            VOID_LOG_INFO("Decoding Delayed frame: {0}", m_CurrentFrame);
            if (m_CurrentFrame == framenumber)
            {
                FillBuffer(pixels);
                break;
            }
        }

        if (ret > framenumber || ret == -1)
        {
            int64_t seek_pts = av_rescale_q(framenumber, av_inv_q(m_Stream->r_frame_rate), m_Stream->time_base);
            // Seek the closest keyframe before the framenumber
            // av_seek_frame(m_Format, m_VideoStreamIndex, seek_pts, AVSEEK_FLAG_BACKWARD);
            // avcodec_flush_buffers(m_CodecContext);
            SeekStream(m_VideoStreamIndex, seek_pts);

            retryCount++;
        }
        else if (ret == framenumber)
        {
            // Frame found, fillup the float input buffer
            found = true;
            FillBuffer(pixels);
            break;
        }
        else if (!seeked)
        {
            int64_t seek_pts = av_rescale_q(framenumber, av_inv_q(m_Stream->r_frame_rate), m_Stream->time_base);
            // /**
            //  * Seek the framenumber
            //  * using AVSEEK_FLAG_FRAME to seek to any frame and don't really care about keyframes
            //  */
            // av_seek_frame(m_Format, m_VideoStreamIndex, seek_pts, AVSEEK_FLAG_FRAME);
            // avcodec_flush_buffers(m_CodecContext);
            SeekStream(m_VideoStreamIndex, seek_pts);

            seeked = true;
        }
    }

    return found;
}

bool FFmpegDecoder::Decode(const std::string& path, const int framenumber, Buffer<unsigned char>& pixels)
{
    /**
     * At the moment, this is not accessible concurrently throught multiple threads
     * Rather than handling this specifically at the cache level, using a guard here
     * TODO: Maybe handling all movies to be single threaded can help solve this in a better way
     * still this shouldn't cause any issues...
     */
    std::lock_guard<std::mutex> guard(m_Mutex);

    // A new movie is being read
    if (path != m_Path)
    {
        Close();
        m_Path = path;
        Open();
    }

    pixels.Resize(av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_Width, m_Height, 1));
    av_image_fill_arrays(m_RGBFrame->data, m_RGBFrame->linesize, pixels.Data(), AV_PIX_FMT_RGB24, m_Width, m_Height, 1);

    bool found = false;
    int retryCount = 0;
    bool seeked = false;
    bool flushed = false;

    // Retry seeking for 3 times before giving up
    while (!found && retryCount < 3)
    {
        // Decode the next frame and it returns back either a negative value or the decoded frame
        v_frame_t ret = DecodeNextFrame();

        /**
         * Then we check if the return value was greater than the requested frame
         * if so, we might need to seek back and try again (max 3 times)
         *
         * Meanwhile also check if the file was at the end of it's frame? and we want to read it again?
         */
        if (ret == -1 && !flushed)
        {
            DecodePacket(m_CodecContext, nullptr);
            VOID_LOG_INFO("Decoding Delayed frame: {0}", m_CurrentFrame);
            flushed = true;
        }

        if (ret > framenumber || ret == -1)
        {
            int64_t seek_pts = av_rescale_q(framenumber, av_inv_q(m_Stream->r_frame_rate), m_Stream->time_base);
            // Seek the closest keyframe before the framenumber
            // av_seek_frame(m_Format, m_VideoStreamIndex, seek_pts, AVSEEK_FLAG_BACKWARD);
            // avcodec_flush_buffers(m_CodecContext);
            SeekStream(m_VideoStreamIndex, seek_pts);

            retryCount++;
        }
        else if (ret == framenumber)
        {
            // Frame found, fillup the float input buffer
            found = true;
            break;
        }
        else if (!seeked)
        {
            int64_t seek_pts = av_rescale_q(framenumber, av_inv_q(m_Stream->r_frame_rate), m_Stream->time_base);
            // /**
            //  * Seek the framenumber
            //  * using AVSEEK_FLAG_FRAME to seek to any frame and don't really care about keyframes
            //  */
            // av_seek_frame(m_Format, m_VideoStreamIndex, seek_pts, AVSEEK_FLAG_FRAME);
            // avcodec_flush_buffers(m_CodecContext);
            SeekStream(m_VideoStreamIndex, seek_pts);
            seeked = true;
        }
    }

    return found;
}

v_frame_t FFmpegDecoder::DecodeNextFrame()
{
    while (av_read_frame(m_Format, m_Packet) >= 0)
    {
        // if (m_Packet->stream_index != m_VideoStreamIndex)
        // {
        //     av_packet_unref(m_Packet);
        //     continue;
        // }

        // int ret = avcodec_send_packet(m_CodecContext, m_Packet);
        // av_packet_unref(m_Packet);

        // if (ret < 0) return -1;
        // while (true)
        // {
        //     ret = avcodec_receive_frame(m_CodecContext, m_Frame);
        //     VOID_LOG_INFO("Legacy -- Return: {0}", ret);
        //     if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        //         break;

        //     if (ret < 0)
        //         return -3;

        //     // m_CurrentFrame = av_rescale_q(m_Frame->pts, m_Stream->time_base, av_inv_q(m_Stream->r_frame_rate));
        //     // sws_scale(m_Scaler, m_Frame->data, m_Frame->linesize, 0, m_Height, m_RGBFrame->data, m_RGBFrame->linesize);
        //     OutputVideoFrame();

        //     av_frame_unref(m_Frame);
        //     return (v_frame_t)m_CurrentFrame;
        // }
        if (m_Packet->stream_index == m_VideoStreamIndex)
        {
            int ret = DecodePacket(m_CodecContext, m_Packet);
            av_packet_unref(m_Packet);
            // if (ret == 0)
            //     return -1;
            if (ret < 0)
                return ret;
    
            VOID_LOG_INFO("Here...");
            return m_CurrentFrame;
        }

        // continue;
    }

    return -1;
}

bool FFmpegDecoder::SeekStream(int index, int64_t timestamp)
{
    if (!m_Format || index < 0)
        return false;
    
    AVStream* stream = m_Format->streams[index];
    const int result = av_seek_frame(m_Format, index, timestamp, AVSEEK_FLAG_BACKWARD);
    if (result < 0)
        return false;

    av_packet_unref(m_Packet);
    av_frame_unref(m_Frame);
    // av_frame_un
    avcodec_flush_buffers(m_CodecContext);

    return true;
}

void FFmpegDecoder::OutputVideoFrame()
{
    m_CurrentFrame = av_rescale_q(m_Frame->pts, m_Stream->time_base, av_inv_q(m_Stream->r_frame_rate));
    sws_scale(m_Scaler, m_Frame->data, m_Frame->linesize, 0, m_Height, m_RGBFrame->data, m_RGBFrame->linesize);    
}

int FFmpegDecoder::DecodePacket(AVCodecContext* dec, const AVPacket* packet)
{
    int ret = 0;
    // submit to the decoder
    ret = avcodec_send_packet(dec, packet);
    if (ret < 0)
    {
        // VOID_LOG_ERROR("Error during decoding {0}", av_err2str(ret));
        return ret;
    }

    // while (ret >= 0)
    // {
    //     ret = avcodec_receive_frame(dec, m_Frame);
    //     if (ret < 0)
    //     {
    //         char buf[512];
    //         // std::string error(av_err2str(ret));
    //         av_strerror(ret, buf, 512);
    //         VOID_LOG_INFO("DecodePacker::Ret: {0} -- Meaning: {1}", ret, buf);
    //         // VOID_LOG_ERROR("Error during decoding {0}", av_err2str(ret));
    //         // No more output frames available but we also don't have any errors
    //         if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
    //             return 0;

    //         // VOID_LOG_ERROR("Error during decoding {0}", av_err2str(ret));
    //         return -3;
    //     }

    //     OutputVideoFrame();
    //     av_frame_unref(m_Frame);
    // }

    ret = avcodec_receive_frame(dec, m_Frame);
    if (ret < 0)
    {
        char buf[512];
        // std::string error(av_err2str(ret));
        av_strerror(ret, buf, 512);
        VOID_LOG_INFO("DecodePacker::Ret: {0} -- Meaning: {1}", ret, buf);
        // VOID_LOG_ERROR("Error during decoding {0}", av_err2str(ret));
        // No more output frames available but we also don't have any errors
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
            return 0;

        // VOID_LOG_ERROR("Error during decoding {0}", av_err2str(ret));
        return -3;
    }

    OutputVideoFrame();
    av_frame_unref(m_Frame);

    return ret;
}

void FFmpegDecoder::FillBuffer(Buffer<float>& out)
{
    float* buffer = out.Data();
    int64_t count = m_Width * m_Height * m_Channels;
    tbb::parallel_for(tbb::blocked_range<int64_t>(0, count), [&](const tbb::blocked_range<int64_t>& r) -> void
    {
        for (int64_t i = r.begin(); i != r.end(); ++i)
            buffer[i] = Linear(m_Buffer[i] / 255.f);
    });
}

VOID_NAMESPACE_CLOSE
