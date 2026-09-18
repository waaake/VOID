// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TIMECODE_H
#define _VOID_TIMECODE_H

/* STD */
#include <string>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct Timecode
{
    v_frame_t frames = 0;
    double framerate = 24.0;
    bool dropframe = false;

    constexpr Timecode() = default;
    constexpr Timecode(v_frame_t frames, double framerate = 24.0, bool dropframe = false)
        : frames(frames), framerate(framerate), dropframe(dropframe) {}

    std::string String() const;
    static Timecode Get(const std::string_view& tc, double framerate, bool dropframe = false);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMECODE_H
