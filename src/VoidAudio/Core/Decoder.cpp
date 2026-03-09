// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <vector>

/* Internal */
#include "Decoder.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Timekeeper.h"

VOID_NAMESPACE_OPEN

AudioDecoder::AudioDecoder()
    : m_Time(-1.0)
    , m_Running(false)
    , m_Finished(false)
    , m_StreamID(-1)
    , m_FormatContext(nullptr)
    , m_CodecContext(nullptr)
    , m_Stream(nullptr)
    , m_Frame(nullptr)
    , m_Packet(nullptr)
    , m_SwrContext(nullptr)
    , m_Valid(false)
    , m_AudioStream(nullptr)
{
}

AudioDecoder::~AudioDecoder()
{
    Cleanup();
}

void AudioDecoder::Init(const std::string& path)
{
    // Free anything from before
    Cleanup();

    if (avformat_open_input(&m_FormatContext, path.c_str(), nullptr, nullptr) < 0)
    return;

    if (avformat_find_stream_info(m_FormatContext, nullptr) < 0)
    return;

    m_StreamID = av_find_best_stream(m_FormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

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

    // Has valid audio to be processed
    m_Valid = true;
    m_AudioStream = new AudioStream(m_CodecContext->sample_rate, m_CodecContext->ch_layout.nb_channels);
}

void AudioDecoder::Start()
{
    if (m_Valid && !m_Running.load())
    {
        m_Running = true;
        if (m_AudioStream->Start())
            m_DecodeWorker = std::async(std::launch::async, &AudioDecoder::DecodeSamples, this);
    }
}

void AudioDecoder::Stop()
{
    if (m_Running.load())
    {
        m_Running = false;

        // Wait till this gets finished
        if (m_DecodeWorker.valid())
            m_DecodeWorker.get();

        m_AudioStream->Stop();
    }
}

void AudioDecoder::Reset()
{
    Timekeeper::Instance().SetTime(-1.0);
    SeekTo(0.0);
}

void AudioDecoder::Cleanup()
{
    m_Valid = false;

    if (m_Frame) av_frame_free(&m_Frame);
    if (m_Packet) av_packet_free(&m_Packet);
    if (m_CodecContext) avcodec_free_context(&m_CodecContext);
    if (m_FormatContext) avformat_close_input(&m_FormatContext);
    if (m_SwrContext) swr_free(&m_SwrContext);

    if (m_AudioStream)
    {
        delete m_AudioStream;
        m_AudioStream = nullptr;
    }
}

// void AudioDecoder::SeekTo(v_frame_t frame)
// {
//     if (m_Valid)
//     {
//         int64_t seek_pts = av_rescale_q(frame, AVRational{1, m_CodecContext->sample_rate}, m_Stream->time_base);
//         if (av_seek_frame(m_FormatContext, m_StreamID, seek_pts, AVSEEK_FLAG_FRAME) >= 0)
//             avcodec_flush_buffers(m_CodecContext);
//     }
// }

void AudioDecoder::SeekTo(double seconds)
{
    if (m_Valid)
    {
        int64_t pts = av_rescale_q(seconds * AV_TIME_BASE, AVRational{1, AV_TIME_BASE}, m_Stream->time_base);
        if (av_seek_frame(m_FormatContext, m_StreamID, pts, AVSEEK_FLAG_FRAME) >= 0)
            avcodec_flush_buffers(m_CodecContext);
    }
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
                    // Update the current time on the Timekeeper, this makes the Video frames sync to this worker thread
                    if (m_AudioStream->WriteSamples(framedata, buffersize, outsamples))
                    {
                        /**
                         * This approach is not good definitely and neither it works fully, just something as a TODO for later
                         * Need to fix this with a better and robust solution
                         */
                        #if __APPLE__
                        std::this_thread::sleep_for(std::chrono::milliseconds((outsamples * 1000 / m_CodecContext->sample_rate) - 5));
                        #else
                        Timekeeper::Instance().SetTime(m_Time.load() - (m_AudioStream->Latency() / 1000));
                        #endif
                    }
                }
            }
        }
        av_packet_unref(m_Packet);
    }
    // m_Finished = true;
}

VOID_NAMESPACE_CLOSE
