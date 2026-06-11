// Copyright (c) 2025 waaake
// Licensed under the MIT License

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
    , m_Pts(0)
{
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
    {
        m_SwsCtx = sws_getContext(
            spec.width,
            spec.height,
            spec.channels == 3 ? AV_PIX_FMT_RGB24 : AV_PIX_FMT_RGBA,
            m_Spec.width,
            m_Spec.height,
            PixelFormat(),
            SWS_BILINEAR,
            nullptr,
            nullptr,
            nullptr
        );
    }

    AVFrame* frame = av_frame_alloc();
    frame->format = m_CodecCtx->pix_fmt;
    frame->width = m_CodecCtx->width;
    frame->height = m_CodecCtx->height;

    // 32 byte align
    if (av_frame_get_buffer(frame, 32) < 0)
        return false;

    if (av_frame_make_writable(frame) < 0)
        return false;

    const uint8_t* srcSlice[1] = { static_cast<const uint8_t*>(buffer) };
    int srcStride[1] = { m_Spec.channels * spec.width };

    sws_scale(m_SwsCtx, srcSlice, srcStride, 0, spec.height, frame->data, frame->linesize);
    frame->pts = m_Pts;

    // Encode
    int ret = avcodec_send_frame(m_CodecCtx, frame);
    if ( ret < 0)
    {
        char errbuf[256];
        av_strerror(ret, errbuf, sizeof(errbuf));
        VOID_LOG_INFO("Unable to send frame: {0}, {1}, {2}", errbuf, ret, ret == AVERROR(EINVAL));
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
    av_frame_free(&frame);
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

VOID_NAMESPACE_CLOSE
