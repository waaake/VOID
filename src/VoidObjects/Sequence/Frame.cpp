// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Frame.h"
#include "VoidObjects/Effects/Effects.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

const FloatImage& SequenceFrame::Image()
{
    if (image->Empty() && frame)
        frame->Image(image);

    return image;
}

void SequenceFrame::Image(FloatImage& image)
{
    if (frame) frame->Image(image);
}

const FloatImage& SequenceFrame::Evaluate()
{
    // Evaluate and clean, unless the effect(s) are updated causing it to be dirty again...
    if (effect && dirty)
    {
        /// TODO: This needs to be better organised and handled implicitly...
        image->CreateEditable();

        effect->Evaluate(image);
        dirty = false;
    }

    return image;
}

void SequenceFrame::Clear()
{
    image->Clear();
    dirty = true;
}

VOID_NAMESPACE_CLOSE
