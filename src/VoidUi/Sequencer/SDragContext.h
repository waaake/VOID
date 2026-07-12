// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_DRAG_CONTEXT_H
#define _SEQUENCER_DRAG_CONTEXT_H

/* Qt */
#include <QPointF>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct SDragContext
{
    bool pressed = false;
    bool active = false;
    QPointF clickpos;
    QPointF scenepos;
    QPointF offset;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_DRAG_CONTEXT_H
