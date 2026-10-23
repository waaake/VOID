// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _AUDIO_BUFFER_H
#define _AUDIO_BUFFER_H

/* STD */
#include <memory>
#include <vector>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct TimeBase
{
    int num;
    int den;
};

struct AudioBuffer
{
    int64_t pts = 0;
    int samplerate = 0;
    int channels = 0;

    // Samples per channel
    int64_t sampleCount = 0;

    // TimeBase
  
    // PCM
    std::vector<float> samples;

    /// @brief Reset the internal state of the buffer, clears underlying samples, if held
    void Reset()
    {
        pts = 0;
        samplerate = 0;
        channels = 0;
        sampleCount = 0;

        samples.clear();
        samples.shrink_to_fit();
    }
};

struct AudioSlice
{
    std::size_t bufindex;

    int64_t startSample;
    int64_t sampleCount;

    int64_t frameOffset;
    int64_t duration;
};

struct AudioFrame
{
    int64_t pts;
    int64_t duration;

    std::vector<AudioSlice> slices;
};

struct FrameAudio
{
    int samplerate;
    int channels;

    int samplecount;

    std::vector<float> samples;
};

typedef std::shared_ptr<FrameAudio> SharedFrameAudio;

///     Video Track (e.g. some 30 FPS)
///    Vid F0      Vid F1     Vid F2    Vid F3
///  |'''''''''||'''''''''||'''''''''||'''''''''|
///
///     Audio Track (e.g 48 KHz)
///     Audio Buffer 0        Audio Buffer 1
///  |''''''''''''''''''''||''''''''''''''''''''|
/// Audio buffer may cross boundaries with multiple video frames

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_BUFFER_H
