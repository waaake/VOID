// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "FFmpegStream.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

FFmpegStream::FFmpegStream(AVFormatContext* format, AVStream* stream, int index, const std::string& path)
    : m_SourcePath(path)
    , m_Format(format)
    , m_Stream(stream)
    , m_Index(index)
    , m_CurrentFrame(-1)
    , m_CurrentAudioSample(-1)
    , m_Scaler(nullptr)
    , m_Resampler(nullptr)
{
}

FFmpegStream::~FFmpegStream()
{
    if (m_Frame) av_frame_free(&m_Frame);
    if (m_CodecContext) avcodec_free_context(&m_CodecContext);
    #if LIBSWSCALE_VERSION_MAJOR < 9
    if (m_Scaler) sws_freeContext(m_Scaler);
    m_Scaler = nullptr;
    #else
    if (m_Scaler) sws_free_context(&m_Scaler);
    #endif
    if (m_Resampler) swr_free(&m_Resampler);
}

bool FFmpegStream::Setup()
{
    m_Frame = av_frame_alloc();
    const AVCodec* codec = avcodec_find_decoder(m_Stream->codecpar->codec_id);
    m_CodecContext = avcodec_alloc_context3(codec);

    avcodec_parameters_to_context(m_CodecContext, m_Stream->codecpar);
    avcodec_open2(m_CodecContext, codec, nullptr);

    if (codec->type == AVMEDIA_TYPE_VIDEO)
    {
        m_Frame->width = m_Stream->codecpar->width;
        m_Frame->height = m_Stream->codecpar->height;
        m_Frame->format = m_CodecContext->pix_fmt;

        return true;
    }
    else if (codec->type == AVMEDIA_TYPE_AUDIO)
    {
        return SetupResampler();
    }

    return false;
}

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
    out.timebase = m_Stream->time_base;
    out.buffer.resize(av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_Frame->width, m_Frame->height, 1));

    int linesizes[4] = { 3 * m_Frame->width, 0, 0, 0 };
    uint8_t* planes[4] = { out.buffer.data(), nullptr, nullptr, nullptr };

    sws_scale(m_Scaler, m_Frame->data, m_Frame->linesize, 0, m_Frame->height, planes, linesizes);
    return true;
}

bool FFmpegStream::Decode(AudioBuffer& out)
{
    if (!m_Resampler || m_Frame->nb_samples <= 0)
        return false;

    std::size_t ret = ResampleAudio(m_Frame, out, -1, m_Frame->sample_rate);
    if (ret == std::string::npos)
        return false;

    if (m_CurrentAudioSample == -1)
        m_CurrentAudioSample = (m_Frame->pts * m_Frame->sample_rate * m_Stream->time_base.num) / m_Stream->time_base.den;
    
    m_CurrentAudioSample += m_Frame->nb_samples;

    return true;
}

bool FFmpegStream::SetupResampler()
{
    AVChannelLayout layout = AV_CHANNEL_LAYOUT_STEREO;
    int ret = swr_alloc_set_opts2(
        &m_Resampler,
        &layout,
        AV_SAMPLE_FMT_FLT,
        48000,
        &m_CodecContext->ch_layout,
        m_CodecContext->sample_fmt,
        m_CodecContext->sample_rate,
        0,
        nullptr
    );

    av_channel_layout_uninit(&layout);
    if (ret < 0 || !m_Resampler)
        return false;

    ret = swr_init(m_Resampler);
    if (ret < 0)
    {
        AVLogError(ret);
        return false;
    }

    return true;
}

std::size_t FFmpegStream::ResampleAudio(AVFrame* frame, AudioBuffer& buffer, int offset, int samplerate) const
{
    const int wanted = frame->nb_samples * samplerate / frame->sample_rate;

    const uint8_t** in = (const uint8_t**)frame->extended_data;
    int outcount = wanted + 256;
    int targetSize = av_samples_get_buffer_size(nullptr, /*target channels*/2, outcount, AV_SAMPLE_FMT_FLT, 0);

    if (targetSize < 0)
        return std::string::npos;

    uint8_t* out;
    if (offset == -1)
    {
        buffer.samples.resize(targetSize);
        out = (uint8_t*)(buffer.samples.data() + buffer.sampleCount * 2 * 2);
    }
    else
    {
        if (targetSize + offset > buffer.samples.size())
            return std::string::npos;

        out = (uint8_t*)(buffer.samples.data() + offset);
    }

    int converted = swr_convert(m_Resampler, &out, outcount, in, frame->nb_samples);
    if (converted < 0)
        return std::string::npos;

    if (offset == -1)
        buffer.sampleCount += converted;

    buffer.pts = m_Frame->pts;
    buffer.samplerate = m_Frame->sample_rate;
    buffer.channels = 2; //m_Frame->ch_layout.nb_channels;

    return converted * 2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT);
}

void AVLogError(int errnum)
{
    char buf[4096];
    av_strerror(errnum, buf, sizeof(buf));
    VOID_LOG_ERROR("AVError: Error code: {} - {}", errnum, buf);
}

VOID_NAMESPACE_CLOSE
