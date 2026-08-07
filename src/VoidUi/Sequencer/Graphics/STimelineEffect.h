// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TIMELINE_EFFECT_H
#define _SEQUENCER_TIMELINE_EFFECT_H

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"

VOID_NAMESPACE_OPEN

class Effect;

class STimelineEffect : public STimelineItem
{
public:
    STimelineEffect(Effect* effect, SequencerContext* context, QGraphicsItem* parent = nullptr);

    Effect* TimelineEffect() const { return m_Effect; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void Update() override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    Effect* m_Effect;

private: /* Methods */
    void CalculateBoundingBox();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TIMELINE_EFFECT_H
