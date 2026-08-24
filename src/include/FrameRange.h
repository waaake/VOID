// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_FRAME_RANGE_H
#define _VOID_FRAME_RANGE_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * This defines the Media Frame Range
 * Members include
 *  startframe
 *  endframe
 *  duration (calculated if not provided)
 */
struct MFrameRange
{
    v_frame_t startframe;
    v_frame_t endframe;
    v_frame_t duration;
    double framerate;

    MFrameRange(v_frame_t start, v_frame_t end, double framerate = 24.0)
        : MFrameRange(start, end, (end - start) + 1, framerate) {}
    MFrameRange(v_frame_t start, v_frame_t end, v_frame_t duration, double framerate = 24.0)
        : startframe(start), endframe(end), duration(duration), framerate(framerate) {}

    bool Overlaps(const MFrameRange& other) const
    {
        return Contains(other.startframe) || Contains(other.endframe);
    }
    bool Covers(const MFrameRange& other) const
    {
        return Contains(other.startframe) && Contains(other.endframe);
    }
    bool Contains(v_frame_t frame) const { return frame >= startframe && frame <= endframe; }
    bool operator==(const MFrameRange& other) const
    {
        return other.startframe == startframe && other.endframe == endframe && other.framerate == framerate;
    }
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_FRAME_RANGE_H
