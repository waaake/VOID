// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MACOS_AUDIO_STREAM_H
#define _MACOS_AUDIO_STREAM_H

/* STD */
#include <cstddef>
#include <mutex>
#include <vector>
#include <condition_variable>

/* MAC */
#include <AudioToolbox/AudioToolbox.h>
// #include <AudioToolbox/AudioQueue.h>


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
    bool WriteSamples(const unsigned char* buffer, std::size_t size);

private:
    int m_Samplerate, m_Channels;
    double m_PlaybackTime, m_SessionTime;
    AudioUnit m_AudioUnit;
    AudioStreamBasicDescription m_Format;
    std::vector<unsigned char> m_Samples;
    std::mutex m_Mutex;

    std::condition_variable m_Cond;   

private:
    static OSStatus RenderCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData);
};

// class AudioStream
// {
// public:
//     AudioStream(int samplerate, int channels);
//     ~AudioStream();

//     bool Start();
//     void Stop();
//     bool WriteSamples(const unsigned char* buffer, std::size_t size);

// private:
//     int m_Samplerate, m_Channels;
//     // AudioUnit m_AudioUnit;
//     AudioQueueRef m_Queue;
//     AudioStreamBasicDescription m_Format;
//     std::vector<unsigned char> m_Samples;
//     std::mutex m_Mutex;

// private:
//     static void OutputCallback(void* inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer);
// };


VOID_NAMESPACE_CLOSE

#endif // _MACOS_AUDIO_STREAM_H
