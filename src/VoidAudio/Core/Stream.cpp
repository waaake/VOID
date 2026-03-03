// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <vector>
#include <thread>

/* Internal */
#include "Stream.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

AudioDecoder::AudioDecoder()
    : m_Time(0.0)
    , m_Running(false)
    , m_Finished(false)
    , m_StreamID(-1)
    , m_FormatContext(nullptr)
    , m_CodecContext(nullptr)
    , m_Stream(nullptr)
    , m_Frame(nullptr)
    , m_Packet(nullptr)
{
}

AudioDecoder::~AudioDecoder()
{
    Cleanup();
}

void AudioDecoder::Init(const std::string& path)
{
    VOID_LOG_INFO("Reading Media: {0}", path);
    Cleanup();
    
    if (avformat_open_input(&m_FormatContext, path.c_str(), nullptr, nullptr) < 0)
    return;
    
    if (avformat_find_stream_info(m_FormatContext, nullptr) < 0)
    return;
    
    m_StreamID = av_find_best_stream(m_FormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    VOID_LOG_INFO("Stream ID: {0}", m_StreamID);
    
    // Does not have audio in it
    if (m_StreamID < 0)
        return;
    
    m_Frame = av_frame_alloc();
    m_Packet = av_packet_alloc();
    m_Stream = m_FormatContext->streams[m_StreamID];
    const AVCodecParameters* codecpar = m_Stream->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);

    m_CodecContext = avcodec_alloc_context3(codec);

    avcodec_parameters_to_context(m_CodecContext, codecpar);
    avcodec_open2(m_CodecContext, codec, nullptr);
    m_SwrContext = swr_alloc();

    AVChannelLayout layout;
    av_channel_layout_default(&layout, m_CodecContext->ch_layout.nb_channels);
    swr_alloc_set_opts2(&m_SwrContext, &layout, AV_SAMPLE_FMT_S16, m_CodecContext->sample_rate, &layout, m_CodecContext->sample_fmt, m_CodecContext->sample_rate, 0, nullptr);
    swr_init(m_SwrContext);

    m_Valid = true;
    VOID_LOG_INFO("Media: {} has valid audio.", path);
}

void AudioDecoder::Start()
{
    VOID_LOG_INFO("Start");
    if (m_Valid && !m_Running.load())
    {
        Reset();
        m_Running = true;
        // Needs to be threaded
        // DecodeSamples();
        std::thread t(&AudioDecoder::DecodeSamples, this);
        VOID_LOG_INFO("Started...");
        t.detach();
    }
}

void AudioDecoder::Stop()
{
    if (m_Running.load())
        m_Running = false;
}

void AudioDecoder::Reset()
{
    if (m_Finished.load())
    {
        if (m_Frame) av_frame_free(&m_Frame);
        if (m_Packet) av_packet_free(&m_Packet);
    
        m_Frame = av_frame_alloc();
        m_Packet = av_packet_alloc();
    
        m_Finished = false;
    }
}

void AudioDecoder::Cleanup()
{
    m_Valid = false;

    if (m_Frame) av_frame_free(&m_Frame);
    if (m_Packet) av_packet_free(&m_Packet);
    if (m_CodecContext) avcodec_free_context(&m_CodecContext);
    if (m_FormatContext) avformat_close_input(&m_FormatContext);
    if (m_SwrContext) swr_free(&m_SwrContext);
}

void AudioDecoder::DecodeSamples()
{
    std::vector<unsigned char> frame;

    while (m_Running.load() && av_read_frame(m_FormatContext, m_Packet) >= 0)
    {
        if (m_Packet->stream_index == m_StreamID)
        {
            if (avcodec_send_packet(m_CodecContext, m_Packet) == 0)
            {
                while (avcodec_receive_frame(m_CodecContext, m_Frame) == 0)
                {
                    m_Time = (double)m_Frame->pts * av_q2d(m_Stream->time_base);
                    int outlines;
                    int outsamples = av_rescale_rnd(m_Frame->nb_samples, m_CodecContext->sample_rate, m_CodecContext->sample_rate, AV_ROUND_UP);
                    int buffersize = av_samples_get_buffer_size(&outlines, m_CodecContext->ch_layout.nb_channels, m_Frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
                    
                    frame.resize(buffersize);
                    unsigned char* framedata = frame.data();

                    swr_convert(m_SwrContext, (uint8_t**)&framedata, outsamples, (const uint8_t**)m_Frame->extended_data, m_Frame->nb_samples);

                    // VOID_LOG_INFO("Time: {0}s, BufferSize: {1}", m_Time.load(), buffersize);
                    std::this_thread::sleep_for(std::chrono::milliseconds(24));
                } 
            }
        }
        av_packet_unref(m_Packet);
    }
    // m_Finished = true;
}

VOID_NAMESPACE_CLOSE
