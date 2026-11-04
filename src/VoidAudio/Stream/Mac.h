// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MACOS_AUDIO_STREAM_H
#define _MACOS_AUDIO_STREAM_H

/* STD */
#include <mutex>
#include <vector>
#include <condition_variable>

/* MAC */
#include <AudioToolbox/AudioToolbox.h>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class AudioStream
{
public:
    AudioStream(int samplerate, int channels);
    ~AudioStream();

    bool Start();
    void Stop();

    bool WriteSamples(const void* buffer, std::size_t size, int samples);
    void Clear();

    void SetVolume(float volume);
    float Volume() const { return m_Volume; }
    bool Muted() const { return m_Volume == 0; }

    double Latency() const;

private:
    int m_Samplerate, m_Channels;
    AudioUnit m_AudioUnit;
    AudioStreamBasicDescription m_Format;
    std::vector<float> m_Samples;
    std::mutex m_Mutex;

    float m_Volume;

private:
    void ApplyVolume(const float* source, float* dest, std::size_t count);
    static OSStatus RenderCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData);
};

VOID_NAMESPACE_CLOSE

#endif // _MACOS_AUDIO_STREAM_H
