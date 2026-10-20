// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "FFmpegStream.h"

VOID_NAMESPACE_OPEN

FFmpegStream::FFmpegStream(AVFormatContext* format, AVStream* stream, int index, const std::string& path)
    : m_SourcePath(path)
    , m_Format(format)
    , m_Stream(stream)
    , m_Index(index)
    , m_CurrentFrame(-1)
{
    m_Frame = av_frame_alloc();
    const AVCodec* codec = avcodec_find_decoder(m_Stream->codecpar->codec_id);
    m_CodecContext = avcodec_alloc_context3(codec);

    avcodec_parameters_to_context(m_CodecContext, m_Stream->codecpar);
    avcodec_open2(m_CodecContext, codec, nullptr);

    m_Frame->width = m_Stream->codecpar->width;
    m_Frame->height = m_Stream->codecpar->height;
    m_Frame->format = m_CodecContext->pix_fmt;

    // int thread_count = 6;
    // if (codec->capabilities & AV_CODEC_CAP_SLICE_THREADS) {
    //     m_CodecContext->thread_count = thread_count;
    //     m_CodecContext->thread_type  = FF_THREAD_SLICE;
    // }
    // if (codec->capabilities & AV_CODEC_CAP_FRAME_THREADS) {
    //     m_CodecContext->thread_count = thread_count;
    //     m_CodecContext->thread_type |= FF_THREAD_FRAME;
    // }
}

FFmpegStream::~FFmpegStream()
{
    if (m_Frame) av_frame_free(&m_Frame);
    if (m_CodecContext) avcodec_free_context(&m_CodecContext);
    if (m_Scaler) sws_free_context(&m_Scaler);
}

// int64_t FFmpegStream::CurrentFrame()
// {
//     // return (m_Frame->best_effort_timestamp * m_Stream->time_base.num) / m_Stream->time_base.den;
//     return m_CurrentFrame;
// }

int64_t FFmpegStream::FrameToPts(v_frame_t frame) const
{
    return av_rescale_q(frame, av_inv_q(m_Stream->r_frame_rate), m_Stream->time_base);
}

void FFmpegStream::SendFlushPacket()
{
    avcodec_send_packet(m_CodecContext, nullptr);
}

int FFmpegStream::SendPacket(AVPacket* packet)
{
    int ret = avcodec_send_packet(m_CodecContext, packet);
    av_packet_unref(packet);
    return ret;
}

int64_t FFmpegStream::ReceiveFrame()
{
    av_frame_unref(m_Frame);
    int ret = avcodec_receive_frame(m_CodecContext, m_Frame);
    if (!ret)
        m_CurrentFrame = av_rescale_q(m_Frame->pts, m_Stream->time_base, av_inv_q(m_Stream->r_frame_rate));

    return ret;
}

void FFmpegStream::FlushBuffers()
{
    avcodec_flush_buffers(m_CodecContext);
}

bool FFmpegStream::Decode(DecodedFrame& out)
{
    m_Scaler = sws_getCachedContext(
            m_Scaler,
            m_Frame->width,
            m_Frame->height,
            m_CodecContext->pix_fmt,
            m_Frame->width,
            m_Frame->height,
            AV_PIX_FMT_RGB24,
            SWS_BILINEAR,
            nullptr,
            nullptr,
            nullptr
        );

    if (!m_Scaler) return false;

    out.framenumber = m_CurrentFrame;
    out.pts = m_Frame->pts;
    out.duration = m_Frame->duration;
    out.timebase = m_Frame->time_base;
    out.buffer.resize(av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_Frame->width, m_Frame->height, 1));

    int linesizes[4] = { 3 * m_Frame->width, 0, 0, 0 };
    uint8_t* planes[4] = { out.buffer.data(), nullptr, nullptr, nullptr };

    sws_scale(m_Scaler, m_Frame->data, m_Frame->linesize, 0, m_Frame->height, planes, linesizes);
    return true;
}

VOID_NAMESPACE_CLOSE
