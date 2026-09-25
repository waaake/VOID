// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TIMECODE_H
#define _VOID_TIMECODE_H

/* STD */
#include <string>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct VOID_API Timecode
{
    v_frame_t frames = 0;
    double framerate = 24.0;
    bool dropframe = false;
    bool negative = false;

    constexpr Timecode() = default;
    constexpr Timecode(v_frame_t frames, double framerate = 24.0, bool dropframe = false)
        : frames(Absolute(frames)), framerate(framerate), dropframe(dropframe), negative(frames < 0) {}

    std::string String() const;
    static Timecode Get(const std::string_view& tc, double framerate, bool dropframe = false);

protected:
    constexpr Timecode(v_frame_t frames, double framerate, bool dropframe, bool negative)
        : frames(frames), framerate(framerate), dropframe(dropframe), negative(negative) {}
    static constexpr v_frame_t Absolute(v_frame_t frames) { return frames < 0 ? -frames : frames; }
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMECODE_H
