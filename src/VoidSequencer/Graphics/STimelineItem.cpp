// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "STimelineItem.h"

VOID_NAMESPACE_OPEN

STimelineItem::STimelineItem(SequencerContext* context, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_Context(context)
{
}

VOID_NAMESPACE_CLOSE
