// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <vector>

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

struct AudioBuffer
{
    std::vector<unsigned char> data;
    int samplerate;
    int channels;
    AudioFormat format;
    int64_t pts;
};


VOID_NAMESPACE_CLOSE
