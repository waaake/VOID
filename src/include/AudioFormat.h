// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <deque>
#include <vector>
#include <map>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * Provides a Mapping to miniaudio format enum
 * ma_format_unknown = 0,
 * ma_format_u8      = 1,
 * ma_format_s16     = 2,
 * ma_format_s24     = 3,
 * ma_format_s32     = 4,
 * ma_format_f32     = 5,
 */
enum class AudioFormat
{
    Unknown,
    U_8,
    S_16,
    S_24,
    S_32,
    F_32,
};

/**
 * An AudioFrame represents the samples of Audio at a given duration
 * The duration is in seconds, which could be easily be converted to frame based
 * on the video framerate for frame syncing
 */
struct AudioFrame
{
    std::vector<unsigned char> data;
    double seconds;
};

struct AudioBuffer
{
    // std::vector<unsigned char> data;
    // std::deque<AudioFrame> data;
    std::map<v_frame_t, std::vector<unsigned char>> data;
    int samplerate;
    int channels;
    AudioFormat format;
    int64_t pts;

    std::vector<unsigned char>& Get(v_frame_t frame)
    {
        if (data.find(frame) == data.end())
        {
            data[frame] = {};
            return data.at(frame);
        }

        return data.at(frame);
    }
};


VOID_NAMESPACE_CLOSE
