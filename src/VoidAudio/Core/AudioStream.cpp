// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <utility>

/* Internal */
#include "AudioStream.h"

VOID_NAMESPACE_OPEN

AudioStream::AudioStream()
{
}

AudioStream::AudioStream(int samplerate, int channels)
{
    #if defined(_WIN32) || defined(__CYGWIN__)
    #elif defined(__linux__)
    // m_Stream = new Stream(samplerate, channels, PA_SAMPLE_S16LE);
    m_Stream = std::move(Stream(samplerate, channels, PA_SAMPLE_S16LE));
    #endif
}

AudioStream::~AudioStream()
{
    m_Stream.Stop();
}

bool AudioStream::Start()
{
    return m_Stream.Start();
}

void AudioStream::Stop()
{
    m_Stream.Stop();
}

bool AudioStream::WriteSamples(const unsigned char* buffer, std::size_t size)
{
    return m_Stream.WriteSamples(buffer, size);   
}

VOID_NAMESPACE_CLOSE
