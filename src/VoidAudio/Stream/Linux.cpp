// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Linux.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

AudioStream::AudioStream(int samplerate, int channels, pa_sample_format_t format)
    : m_Stream(nullptr)
{
    m_SampleSpec.format = format;
    m_SampleSpec.rate = samplerate;
    m_SampleSpec.channels = channels;

    VOID_LOG_INFO("New Audio Stream with {0} channels @ {1} bps", channels, samplerate);
}


AudioStream::~AudioStream()
{
    Stop();
}

bool AudioStream::Start()
{
    int error;
    m_Stream = pa_simple_new(nullptr, "VoidAudio", PA_STREAM_PLAYBACK, nullptr, "Playback", &m_SampleSpec, nullptr, nullptr, &error);

    if (!m_Stream)
    {
        VOID_LOG_ERROR("Pulseaudio init failed: {0}", pa_strerror(error));
        return false;
    }

    return true;
}

void AudioStream::Stop()
{
    if (m_Stream)
    {
        int error;
        pa_simple_flush(m_Stream, &error);
        pa_simple_free(m_Stream);
    }

    m_Stream = nullptr;
}

double AudioStream::Latency() const
{
    if (m_Stream)
    {
        int error;
        uint64_t latency = pa_simple_get_latency(m_Stream, &error);
    
        return latency / 1000.0;
    }

    return 0.0;
}

bool AudioStream::WriteSamples(const unsigned char* data, std::size_t size, int samples)
{
    if (!m_Stream)
        return false;

    int error;
    if (pa_simple_write(m_Stream, data, size, &error) < 0)
    {
        VOID_LOG_INFO("Unable to write samples: {0}", pa_strerror(error));
        return false;
    }

    return true;
}

VOID_NAMESPACE_CLOSE
