// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_PREVIEW_TRACKITEM_H
#define _SEQUENCER_PREVIEW_TRACKITEM_H

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

class SPreviewTrackItem : public STimelineItem
{
public:
    SPreviewTrackItem(const SharedTrackItem& item, SequencerContext* context, QGraphicsItem* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    SharedTrackItem TrackItem() const { return m_Item; }
    void Update();
    void UpdatePosition(int x, int y);

private:
    SharedTrackItem m_Item;

private: /* Methods */
    void CalculateBoundingBox();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_PREVIEW_TRACKITEM_H
