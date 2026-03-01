// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _AUDIO_STREAM_H
#define _AUDIO_STREAM_H

/* Internal */
#if defined(_WIN32) || defined(__CYGWIN__)
#elif defined(__linux__)
#include "VoidAudio/Platform/LinuxStream.h"
#endif

#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API AudioStream
{
public:
    AudioStream();
    AudioStream(int samplerate, int channels);
    ~AudioStream();

    bool Start();
    void Stop();
    bool WriteSamples(const unsigned char* buffer, std::size_t size);

private: /* Members */
#if defined(_WIN32) || defined(__CYGWIN__)
#elif defined(__linux__)
    Stream m_Stream;
#endif
};

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_STREAM_H
