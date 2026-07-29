// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TRACK_ITEM_H
#define _SEQUENCER_TRACK_ITEM_H

// /* Qt */
// #include <QGraphicsObject>

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"
#include "SHandleItem.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

// class SequencerContext;
class STrack;

class STrackItem : public STimelineItem
{
public:
    STrackItem(const SharedTrackItem& item, SequencerContext* context, QGraphicsItem* parent = nullptr);

    SharedTrackItem TrackItem() const { return m_Item; }
    bool Selected() const { return m_Context->SelectionModel()->IsSelected(m_Item); }

    STrack* Track() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void Update() override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    SSlipClipContext m_SlipContext;
    SharedTrackItem m_Item;
    SHandleItem *m_HeadHandle, *m_TailHandle;

private: /* Methods */
    void CalculateBoundingRect();
    void ToggleHandles(bool visible);
    QColor Background(const QStyleOptionGraphicsItem* option) const;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TRACK_ITEM_H
