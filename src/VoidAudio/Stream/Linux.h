// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _LINUX_AUDIO_STREAM_H
#define _LINUX_AUDIO_STREAM_H

/* STD */
#include <cstddef>

/* Pulse */
#include <pulse/error.h>
#include <pulse/simple.h>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class AudioStream
{
public:
    AudioStream(int samplerate, int channels, pa_sample_format_t format = PA_SAMPLE_S16LE);
    ~AudioStream();

    bool Start();
    void Stop();
    bool WriteSamples(const unsigned char* data, std::size_t size);

private:
    pa_simple* m_Stream;
    pa_sample_spec m_SampleSpec;
};

VOID_NAMESPACE_CLOSE

#endif // _LINUX_AUDIO_STREAM_H
