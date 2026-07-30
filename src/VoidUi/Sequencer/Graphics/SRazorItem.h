// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_RAZOR_ITEM_H
#define _SEQUENCER_RAZOR_ITEM_H

/* Internal */
#include "STimelineItem.h"

VOID_NAMESPACE_OPEN

class SRazorItem : public STimelineItem
{
public:
    explicit SRazorItem(SequencerContext* context, QGraphicsItem* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void SetX(int x);
};

class STrackRazorItem : public SRazorItem
{
public:
    explicit STrackRazorItem(SequencerContext* context, QGraphicsItem* parent = nullptr);
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_RAZOR_ITEM_H
