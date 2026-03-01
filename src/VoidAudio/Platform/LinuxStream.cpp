// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include "LinuxStream.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

Stream::Stream()
    : m_Stream(nullptr)
{
    m_SampleSpec.format = PA_SAMPLE_S16LE;
    m_SampleSpec.rate = 44100;
    // m_SampleSpec.rate = 48000;
    m_SampleSpec.channels = 2;
    // m_SampleSpec.channels = 6;
}

Stream::Stream(int samplerate, int channels, pa_sample_format_t format)
    : m_Stream(nullptr)
{
    m_SampleSpec.format = format;
    m_SampleSpec.rate = samplerate;
    m_SampleSpec.channels = channels;
}

Stream::~Stream()
{
    Stop();
}

bool Stream::Start()
{
    if (m_Stream)
        return true;

    int error;
    m_Stream = pa_simple_new(
        nullptr,
        "VoidStream",
        PA_STREAM_PLAYBACK,
        nullptr,
        "Playback",
        &m_SampleSpec,
        nullptr,
        nullptr,
        &error
    );

    if (!m_Stream)
    {
        VOID_LOG_ERROR("Pulseaudio init failed: {0}", pa_strerror(error));
        return false;
    }

    return true;
}

void Stream::Stop()
{
    if (!m_Stream)
        return;

    pa_simple_flush(m_Stream, nullptr);
    pa_simple_drain(m_Stream, nullptr);
    pa_simple_free(m_Stream);
}

bool Stream::WriteSamples(const unsigned char* buffer, std::size_t size)
{
    if (!m_Stream)
        return false;

    int error;

    if (pa_simple_write(m_Stream, buffer, size, &error) < 0)
    {
        VOID_LOG_INFO("Unable to write samples to PulseAudio: {0}", pa_strerror(error));
        return false;
    }
    return true;
}

VOID_NAMESPACE_CLOSE
