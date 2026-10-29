// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _LINUX_AUDIO_STREAM_H
#define _LINUX_AUDIO_STREAM_H

/* STD */
#include <vector>

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
    bool WriteSamples(const unsigned char* data, std::size_t size, int samples);
    bool WriteSamples(const void* data, std::size_t bytes, int samples);
    void SeekTo(double time) { }

    void SetVolume(float volume);
    double CurrentTime() const { return 0.0; }
    double Latency() const;

private:
    std::vector<float> m_InternalBuffer;
    pa_sample_spec m_SampleSpec;
    pa_simple* m_Stream;
    float m_Volume;

private:
    void Process(const float* source, float* buffer, std::size_t count);
};

VOID_NAMESPACE_CLOSE

#endif // _LINUX_AUDIO_STREAM_H
