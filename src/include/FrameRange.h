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
public: /* Members */
    v_frame_t startframe;
    v_frame_t endframe;
    v_frame_t duration;

public: /* Methods */
    MFrameRange(v_frame_t start, v_frame_t end) : MFrameRange(start, end, (end - start) + 1) {}
    MFrameRange(v_frame_t start, v_frame_t end, v_frame_t duration) : startframe(start), endframe(end), duration(duration) {}
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_FRAME_RANGE_H
