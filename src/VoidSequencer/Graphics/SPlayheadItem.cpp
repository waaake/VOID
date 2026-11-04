// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QPalette>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "SPlayheadItem.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SPlayheadItem::SPlayheadItem(SequencerContext* context, QGraphicsItem* item)
    : STimelineItem(context, item)
{
    m_BoundingRect = QRectF(-1, 0, 2, Sequencer::SceneHeight * 2);
    setPos(m_Context->Geometry()->FrameToSceneX(m_Context->Controller()->CurrentFrame()), -Sequencer::SceneHeight);
    setZValue(Sequencer::ZPlayheadItem);
}

void SPlayheadItem::Update()
{
    setPos(m_Context->Geometry()->FrameToSceneX(m_Context->Controller()->CurrentFrame()), -Sequencer::SceneHeight);
    update();
}

void SPlayheadItem::Update(v_frame_t frame)
{
    setPos(m_Context->Geometry()->FrameToSceneX(frame), -Sequencer::SceneHeight);
    update();
}

void SPlayheadItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(QPen(option->palette.color(QPalette::Highlight), 1));
    painter->drawLine(QPoint(0, 0), QPoint(0, boundingRect().height()));
}

VOID_NAMESPACE_CLOSE
