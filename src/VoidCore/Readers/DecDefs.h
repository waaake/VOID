// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _DECODER_DEFINITIONS_H
#define _DECODER_DEFINITIONS_H

/* FFmpeg */
extern "C" {
#include <libavutil/rational.h>
}

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct DecodedFrame
{
    std::vector<uint8_t> buffer;
    AVRational timebase{0, 1};
    int64_t pts = -1;
    int64_t duration = 0;
    int64_t framenumber = -1;
};

struct TimeFrame
{
    int64_t pts = -1;
    int64_t duration = -1;
    AVRational timebase;

    inline explicit operator bool() const { return Valid(); }
    bool Valid() const { return pts > -1; }
    bool Invalid() const { return pts < 0; }
};

VOID_NAMESPACE_CLOSE

#endif // _DECODER_DEFINITIONS_H
