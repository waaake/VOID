// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_DRAG_CONTEXT_H
#define _SEQUENCER_DRAG_CONTEXT_H

/* Qt */
#include <QPointF>
#include <QRect>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct SDragContext
{
    QPointF clickpos;
    QPointF scenepos;
    QPointF offset;
    bool pressed = false;
    bool active = false;
};

struct SMarqueeContext
{
    QPoint clickpos;
    QRect rect;
    bool pressed = false;
    bool active = false;
};

struct SSlipClipContext
{
    QPointF sourcepos;
    int offset = 0;
    bool pressed = false;
    bool active = false;
};

struct SItemTrimContext
{
    enum Type { HEAD, TAIL };
    QPointF sourcepos;
    int handle = 0;
    Type type;
    bool pressed = false;
    bool active = false;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_DRAG_CONTEXT_H
