// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "STimelineItem.h"

VOID_NAMESPACE_OPEN

class SPlayheadItem : public STimelineItem
{
public:
    explicit SPlayheadItem(SequencerContext* context, QGraphicsItem* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void Update() override;
};

VOID_NAMESPACE_CLOSE
