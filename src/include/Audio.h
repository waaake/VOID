// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _AUDIO_BUFFER_H
#define _AUDIO_BUFFER_H

/* STD */
#include <cstring>
#include <memory>
#include <vector>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

// struct TimeBase
// {
//     int num;
//     int den;
// };

/// @brief Direct mapped chunk of PCM from ffmpeg's audio stream
/// Stores the Audio buffer's pts, samplerate and channels
struct AudioBuffer
{
    int64_t pts = 0;
    int samplerate = 0;
    int channels = 0;

    // Samples per channel
    int64_t sampleCount = 0;

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

    /// @brief Copyies n samples from the internal buffer to the provided out buffer. The out buffer must
    /// have enough space to store the requested n samples, else this might lead to undefined behaviour.
    /// @param _n Number of samples to be copied from the buffer.
    /// @param start Sample to start copying from.
    /// @param buffer Output buffer.
    void CopySamples(std::size_t _n, std::size_t start, float* buffer) const
    {
        // std::copy_n(samples.data() + start, _n, buffer);
        std::memcpy(buffer, samples.data() + start, _n * sizeof(float));
    }
};

/// @brief FrameAudio is a representation of samples aligned per video frame
/// this is calculated from the video frame's pts and duration when represented in audio sample frames
///
/// An e.g. of a movie with a constant framerate of 60 fps and a timebase of 1/15360 ticks, provides us with
/// frame duration of 256 ticks
/// Internally we're outputing the audio as 2 channels and 48000 Hz, so based on the resulting calculations,
/// each video ticks correspond to 800 samples per channel, which becomes 1600 samples for 2 channels
///
/// Frames may get audio shared between audio buffers based on how the audio and video frames overlap.
///
///     Video Track (e.g. some 60 FPS)
///    Vid F0      Vid F1     Vid F2    Vid F3
///  |'''''''''||'''''''''||'''''''''||'''''''''|
///
///     Audio Track (e.g 48 KHz)
///     Audio Buffer 0        Audio Buffer 1
///  |''''''''''''''''''''||''''''''''''''''''''|
/// Audio buffer may cross boundaries with multiple video frames
struct FrameAudio
{
    int samplerate;
    int channels;

    int samplecount;

    std::vector<float> samples;

    std::size_t Bytes() const { return samples.size() * sizeof(float); }
    std::size_t Size() const { return samples.size(); }
    const void* Data() const { return samples.data(); }
};

typedef std::shared_ptr<FrameAudio> SharedFrameAudio;

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_BUFFER_H
