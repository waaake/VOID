// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_HANDLE_ITEM_H
#define _SEQUENCER_HANDLE_ITEM_H

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"

VOID_NAMESPACE_OPEN

class SHandleItem : public STimelineItem
{
public:
    SHandleItem(int handle, SequencerContext* context, QGraphicsItem* parent = nullptr);
    void Update();
    void Update(int handle);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    int m_Handle;

private: /* Methods */
    void CalculateBoundingRect();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_HANDLE_ITEM_H
