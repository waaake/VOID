// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _AUDIO_STREAM_LINUX_H
#define _AUDIO_STREAM_LINUX_H

/* STD */
#include <cstddef>

/* Pulse Audio */
#include <pulse/error.h>
#include <pulse/simple.h>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class Stream
{
public:
    Stream();
    Stream(int samplerate, int channels, pa_sample_format_t format);
    ~Stream();

    bool Start();
    void Stop();
    bool WriteSamples(const unsigned char* buffer, std::size_t size);

private: /* Members */
    pa_simple* m_Stream;
    pa_sample_spec m_SampleSpec;
};

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_STREAM_LINUX_H
