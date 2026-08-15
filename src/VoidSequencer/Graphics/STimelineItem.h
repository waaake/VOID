// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TIMELINE_ITEM_H
#define _SEQUENCER_TIMELINE_ITEM_H

/* Qt */
#include <QGraphicsObject>
#include <QRectF>

/* Internal */
#include "Definition.h"
#include "VoidSequencer/SContext.h"
#include "VoidSequencer/SDragContext.h"

VOID_NAMESPACE_OPEN

class STimelineItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit STimelineItem(SequencerContext* context, QGraphicsItem* parent = nullptr);
    virtual QRectF boundingRect() const override { return m_BoundingRect; }

    virtual void Update() {}

protected:
    QRectF m_BoundingRect;
    SDragContext m_Drag;
    SequencerContext* m_Context;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TIMELINE_ITEM_H
