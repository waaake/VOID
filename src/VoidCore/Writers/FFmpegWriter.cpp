// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "FFmpegWriter.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

FFmpegWriter::FFmpegWriter(int width, int height, int channels, const WriterType& type)
    : PixWriter(width, height, channels, type)
    , m_FormatCtx(nullptr)
    , m_CodecCtx(nullptr)
    , m_SwsCtx(nullptr)
    , m_Frame(0)
{
}

bool FFmpegWriter::Setup(const std::string& path)
{
    if (path != m_Path)
    {
        Cleanup();
        m_Path = path;
        VOID_LOG_INFO("FFmpegWriter::Setup path: {0}", m_Path);

        avformat_alloc_output_context2(&m_FormatCtx, nullptr, nullptr, path.c_str());

        // H.264 encode
        // TODO: Enable multi format selection for Movies
        // How to best implement that?
        const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        AVStream* stream = avformat_new_stream(m_FormatCtx, codec);

        m_CodecCtx = avcodec_alloc_context3(codec);

        m_CodecCtx->codec_id = AV_CODEC_ID_H264;
        m_CodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
        m_CodecCtx->width = m_Width;
        m_CodecCtx->height = m_Height;
        m_CodecCtx->time_base = {1, 30};
        m_CodecCtx->framerate = {30, 1};
        m_CodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

        avcodec_open2(m_CodecCtx, codec, nullptr);
        avcodec_parameters_from_context(stream->codecpar, m_CodecCtx);

        // open
        avio_open(&m_FormatCtx->pb, path.c_str(), AVIO_FLAG_WRITE);
        if (avformat_write_header(m_FormatCtx, nullptr) > 0)
        {
            VOID_LOG_WARN("Unable to write header.");

            Cleanup();
            return false;
        }

        m_SwsCtx = sws_getContext(m_Width, m_Height, AV_PIX_FMT_RGBA, m_Width, m_Height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, nullptr, nullptr, nullptr);

        return true;
    }

    return false;
}

void FFmpegWriter::AddBuffer(const void* buffer, std::size_t size, const BufferType& type)
{
    if (!m_FormatCtx)
        return;

    AVFrame* frame = av_frame_alloc();
    frame->format = m_CodecCtx->pix_fmt;
    frame->width = m_CodecCtx->width;
    frame->height = m_CodecCtx->height;
    
    // 32 byte align
    av_frame_get_buffer(frame, 32);

    const uint8_t* srcSlice[1] = { static_cast<const uint8_t*>(buffer) };
    int srcStride[1] = { 4 * m_Width };

    sws_scale(m_SwsCtx, srcSlice, srcStride, 0, m_Height, frame->data, frame->linesize);
    frame->pts = m_Frame;

    // Encode
    avcodec_send_frame(m_CodecCtx, frame);
    AVPacket* packet = av_packet_alloc();
    // av_init_packet(packet);
    if (avcodec_receive_packet(m_CodecCtx, packet) == 0)
    {
        av_interleaved_write_frame(m_FormatCtx, packet);
        av_packet_unref(packet);
    }

    av_frame_free(&frame);
    m_Frame++;
}

bool FFmpegWriter::Write()
{
    if (!m_FormatCtx)
        return false;

    av_write_trailer(m_FormatCtx);
    avio_close(m_FormatCtx->pb);

    return true;
}

void FFmpegWriter::Cleanup()
{
    #if LIBSWSCALE_VERSION_MAJOR < 9
    if (m_SwsCtx) sws_freeContext(m_SwsCtx);
    m_SwsCtx = nullptr;
    #else
    if (m_SwsCtx) sws_free_context(&m_SwsCtx);
    #endif
    
    if (m_CodecCtx) avcodec_free_context(&m_CodecCtx);
    if (m_FormatCtx)
    {
        avformat_free_context(m_FormatCtx);
        m_FormatCtx = nullptr;
    }

    m_Frame = 0;
}

VOID_NAMESPACE_CLOSE
