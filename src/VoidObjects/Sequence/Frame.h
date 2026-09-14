// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCE_FRAME_H
#define _SEQUENCE_FRAME_H

/* Internal */
#include "Definition.h"
#include "Image.h"
#include "VoidCore/Media/Frame.h"

VOID_NAMESPACE_OPEN

class Effect;
class TrackItem;

/**
 * @brief Describes a Frame in the context of a Sequence
 * Holds the underlying Media Frame it points to and an image buffer
 * Also keeps track of the track item this belongs to.
 * 
 */
struct SequenceFrame
{
    FloatImage image;
    TrackItem* item;
    Frame* frame;
    // This effect is the top-most effect applied on the item
    // Any effects below (in the visual stack) are parent (or grand-parents) to this effect
    Effect* effect;
    bool dirty;

    SequenceFrame() : SequenceFrame(nullptr, nullptr) {}
    SequenceFrame(TrackItem* item, Frame* frame, Effect* effect = nullptr)
        : image(VOID_NAMESPACE::Image<float>::Create()), item(item), frame(frame), effect(effect), dirty(true) {}

    bool Valid() const noexcept { return (bool)frame; }
    explicit operator bool() const noexcept { return (bool)frame; }

    FloatImage Image();
    void Image(FloatImage& image);
    FloatImage Evaluate();
    void Clear();
    void SetDirty() { dirty = true; }
    void SetEffect(Effect* effect) { this->effect = effect; }
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCE_FRAME_H
