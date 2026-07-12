// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TRACK_ITEM_H
#define _SEQUENCER_TRACK_ITEM_H

// /* Qt */
// #include <QGraphicsObject>

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

// class SequencerContext;
class STrack;

class STrackItem : public STimelineItem
{
    // Q_OBJECT
public:
    STrackItem(const SharedTrackItem& item, SequencerContext* context, QGraphicsItem* parent = nullptr);

    // QRectF boundingRect() const override;
    // v_frame_t TimelineIn() const { return m_Item->TimelineIn(); }
    // v_frame_t TimelineOut() const { return m_Item->TimelineOut(); }

    // SharedTrackItem TrackItem() const { m_Item; }

    STrack* Track() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void Update() override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    SharedTrackItem m_Item;
    // SequencerContext* m_Context;
    // QRectF m_BoundingRect;

private: /* Methods */
    void CalculateBoundingRect();
};

// class TrackItemRenderer

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TRACK_ITEM_H
