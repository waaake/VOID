// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>
#include <cstdint>
#include <cstring>

/* FFmpeg */
extern "C" {
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

/* Internal */
#include "FFmpegWriter.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

FFmpegWriter::FFmpegWriter(const EncodeSpec& spec)
    : PixWriter(spec)
    , m_FormatCtx(nullptr)
    , m_CodecCtx(nullptr)
    , m_SwsCtx(nullptr)
    , m_Stream(nullptr)
    , m_SourceFrame(nullptr)
    , m_DestinationFrame(nullptr)
    , m_Pts(0)
{
}

FFmpegWriter::~FFmpegWriter()
{
    Cleanup();
}

bool FFmpegWriter::Setup(const std::string& path)
{
    if (path != m_Path)
    {
        Cleanup();
        m_Path = path;

        avformat_alloc_output_context2(&m_FormatCtx, nullptr, nullptr, path.c_str());
        const AVCodec* codec = avcodec_find_encoder(Codec());
        m_CodecCtx = avcodec_alloc_context3(codec);

        m_CodecCtx->codec_id = Codec();
        m_CodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
        m_CodecCtx->width = m_Spec.width;
        m_CodecCtx->height = m_Spec.height;
        m_CodecCtx->time_base = av_make_q(1, m_Spec.rate);
        m_CodecCtx->framerate = av_make_q(m_Spec.rate, 1);

        // Configure codec context params based on user specified out codec type
        ContextSetup();

        int ret = avcodec_open2(m_CodecCtx, codec, nullptr);
        if (ret < 0)
        {
            VOID_LOG_WARN("Unable to open specified codec");
            Cleanup();
            return false;
        }

        m_Stream = avformat_new_stream(m_FormatCtx, codec);
        avcodec_parameters_from_context(m_Stream->codecpar, m_CodecCtx);
        // This will change when we call avformat_write_header to something which ffmpeg finds appropriate
        // to ensure this the framerate of the final written container, ensure the the AVFrame* pts is
        // always incremented by av_rescale_q(1, codec_context->time_base, stream->time_base)
        m_Stream->time_base = av_make_q(1, m_Spec.rate);

        // open
        avio_open(&m_FormatCtx->pb, path.c_str(), AVIO_FLAG_WRITE);
        if (avformat_write_header(m_FormatCtx, nullptr))
        {
            VOID_LOG_WARN("Unable to write header");
            Cleanup();
            return false;
        }
        return true;
    }

    return false;
}

bool FFmpegWriter::AddBuffer(const void* buffer, std::size_t size, const InputSpec& spec)
{
    if (!m_FormatCtx)
        return false;

    if (!m_SwsCtx)
        InitFrameContext(spec);

    switch (spec.type)
    {
        case BufferType::Float:
            CopyBuffer(static_cast<const float*>(buffer), m_SourceFrame->data[0], size);
            break;
        case BufferType::Uint16:
            CopyBuffer(static_cast<const uint16_t*>(buffer), m_SourceFrame->data[0], size);
            break;
        case BufferType::Uint8:
        default:
            CopyBuffer(static_cast<const uint8_t*>(buffer), m_SourceFrame->data[0], size);
    }

    sws_scale(
        m_SwsCtx,
        m_SourceFrame->data, m_SourceFrame->linesize,
        0, spec.height,
        m_DestinationFrame->data, m_DestinationFrame->linesize
    );

    m_DestinationFrame->pts = m_Pts;

    int ret = avcodec_send_frame(m_CodecCtx, m_DestinationFrame);
    if (ret < 0)
    {
        char err[256];
        av_strerror(ret, err, sizeof(err));
        VOID_LOG_ERROR("Unable to send frame: {0}, {1}, {2}", err, ret, ret == AVERROR(EINVAL));
        return false;
    }

    AVPacket* packet = av_packet_alloc();
    while (avcodec_receive_packet(m_CodecCtx, packet) == 0)
    {
        av_packet_rescale_ts(packet, m_CodecCtx->time_base, m_Stream->time_base);
        packet->stream_index = m_Stream->index;

        av_interleaved_write_frame(m_FormatCtx, packet);
        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    m_Pts++;

    return true;
}

bool FFmpegWriter::Write()
{
    if (!m_FormatCtx)
        return false;

    avcodec_send_frame(m_CodecCtx, nullptr);
    AVPacket* packet = av_packet_alloc();

    while (avcodec_receive_packet(m_CodecCtx, packet) == 0)
    {
        av_packet_rescale_ts(packet, m_CodecCtx->time_base, m_Stream->time_base);
        av_interleaved_write_frame(m_FormatCtx, packet);
        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    av_write_trailer(m_FormatCtx);
    avio_close(m_FormatCtx->pb);

    return true;
}

void FFmpegWriter::Cleanup()
{
    if (m_SourceFrame) av_frame_free(&m_SourceFrame);
    if (m_DestinationFrame) av_frame_free(&m_DestinationFrame);
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

    m_Pts = 0;
}

AVCodecID FFmpegWriter::Codec()
{
    switch (m_Spec.codec)
    {
        case MovieCodec::DNXHD: return AV_CODEC_ID_DNXHD;
        case MovieCodec::MJPEG: return AV_CODEC_ID_MJPEG;
        case MovieCodec::MPEG4: return AV_CODEC_ID_MPEG4;
        case MovieCodec::PRORES: return AV_CODEC_ID_PRORES;
        case MovieCodec::H264:
        default: return AV_CODEC_ID_H264;
    }
}

AVPixelFormat FFmpegWriter::PixelFormat()
{
    switch (m_Spec.codec)
    {
        case MovieCodec::DNXHD: return AV_PIX_FMT_YUV422P;
        case MovieCodec::PRORES: return AV_PIX_FMT_YUV422P10LE;
        case MovieCodec::MJPEG: return AV_PIX_FMT_YUV422P;
        case MovieCodec::MPEG4:
        case MovieCodec::H264:
        default: return AV_PIX_FMT_YUV420P;
    }
}

void FFmpegWriter::ContextSetup()
{
    switch (m_Spec.codec)
    {
        case MovieCodec::PRORES:
            m_CodecCtx->pix_fmt = AV_PIX_FMT_YUV422P10LE;

            av_opt_set(m_CodecCtx->priv_data, "profile", "hq", 0);
            break;

        case MovieCodec::DNXHD:
            m_CodecCtx->pix_fmt = AV_PIX_FMT_YUV422P;
            m_CodecCtx->bit_rate = 145000000;
            break;

        case MovieCodec::MJPEG:
            m_CodecCtx->pix_fmt = AV_PIX_FMT_YUV422P;
            m_CodecCtx->bit_rate = 50000000;
            m_CodecCtx->color_range = AVCOL_RANGE_JPEG;
            break;

        case MovieCodec::MPEG4:
            m_CodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
            m_CodecCtx->bit_rate = 10000000;
            break;

        case MovieCodec::H264:
        default:
            m_CodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
            // m_CodecCtx->gop_size = m_Spec.rate * 2;
            // m_CodecCtx->max_b_frames = 2;
            
            // av_opt_set(m_CodecCtx->priv_data, "crf", "18", 0);
            break;
    }
}

void FFmpegWriter::InitFrameContext(const InputSpec& spec)
{
    m_SourceFrame = av_frame_alloc();
    m_SourceFrame->width = spec.width;
    m_SourceFrame->height = spec.height;

    // At the moment, specifically about the 4 channel inputs, AV_PIX_FMT_RGBAF32 doesn't seem to work
    // So using standard unsigned char buffers with RGB and RGBA formats and safely casting the data from
    // float and uint16_t (unsigned short) buffers to unsigned char buffers for writing out
    m_SourceFrame->format = spec.channels == 3 ? AV_PIX_FMT_RGB24 : AV_PIX_FMT_RGBA;

    av_image_alloc(
        m_SourceFrame->data,
        m_SourceFrame->linesize,
        spec.width,
        spec.height,
        (AVPixelFormat)m_SourceFrame->format,
        32
    );

    m_DestinationFrame = av_frame_alloc();
    m_DestinationFrame->width = m_Spec.width;
    m_DestinationFrame->height = m_Spec.height;
    m_DestinationFrame->format = m_CodecCtx->pix_fmt;

    av_image_alloc(
        m_DestinationFrame->data,
        m_DestinationFrame->linesize,
        m_Spec.width,
        m_Spec.height,
        (AVPixelFormat)m_DestinationFrame->format,
        32
    );

    m_SwsCtx = sws_getContext(
        m_SourceFrame->width,
        m_SourceFrame->height,
        (AVPixelFormat)m_SourceFrame->format,
        m_DestinationFrame->width,
        m_DestinationFrame->height,
        (AVPixelFormat)m_DestinationFrame->format,
        SWS_BICUBIC,
        nullptr,
        nullptr,
        nullptr
    );
}

void FFmpegWriter::CopyBuffer(const uint8_t* src, uint8_t* dest, std::size_t size)
{
    std::memcpy(dest, src, size);
}

void FFmpegWriter::CopyBuffer(const uint16_t* src, uint8_t* dest, std::size_t size)
{
    for (int i = 0; i < static_cast<int>(size); ++i)
        dest[i] = static_cast<uint8_t>(src[i] / (UINT16_MAX / UINT8_MAX));
}

void FFmpegWriter::CopyBuffer(const float* src, uint8_t* dest, std::size_t size)
{
    for (int i = 0; i < static_cast<int>(size); ++i)
        dest[i] = static_cast<uint8_t>(std::clamp(src[i], 0.f, 1.f) * 255.f);
}

VOID_NAMESPACE_CLOSE
