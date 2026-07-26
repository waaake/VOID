// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_HANDLE_ITEM_H
#define _SEQUENCER_HANDLE_ITEM_H

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"

VOID_NAMESPACE_OPEN

enum class HandleType
{
    HEAD,
    TAIL
};

class SHandleItem : public STimelineItem
{
public:
    SHandleItem(const SharedTrackItem& item, const HandleType& type, SequencerContext* context, QGraphicsItem* parent = nullptr);
    void Update();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    SharedTrackItem m_Item;
    HandleType m_Type;

private: /* Methods */
    void CalculateBoundingRect();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_HANDLE_ITEM_H
