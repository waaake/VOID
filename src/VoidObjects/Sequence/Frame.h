// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCE_FRAME_H
#define _SEQUENCE_FRAME_H

/* Internal */
#include "Definition.h"
#include "Image.h"
#include "VoidCore/Media/Frame.h"

VOID_NAMESPACE_OPEN

class TrackItem;

/**
 * @brief Describes a Frame in the context of a Sequence
 * Holds the underlying Media Frame it points to and an image buffer
 * Also keeps track of the track item this belongs to.
 * 
 */
struct SequenceFrame
{
    // Image image;
    TrackItem* item;
    Frame* frame;

    SequenceFrame() : item(nullptr), frame(nullptr) {}
    SequenceFrame(TrackItem* item, Frame* frame) : item(item), frame(frame) {}

    bool Valid() const noexcept { return (bool)frame; }
    explicit operator bool() const noexcept { return (bool)frame; }

    FloatImage Image() { return frame ? frame->Image() : nullptr; }
    void Image(FloatImage& image) { if (frame) frame->Image(image); }
    void Clear() { if (frame) frame->Clear(false); }
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCE_FRAME_H
