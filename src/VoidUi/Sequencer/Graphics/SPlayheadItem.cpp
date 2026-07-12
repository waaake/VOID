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
    m_BoundingRect = QRectF(-1, 0, 2, Sequencer::SceneHeight);
    setZValue(Sequencer::ZPlayheadItem);
}

void SPlayheadItem::Update()
{
    prepareGeometryChange();
    setPos(m_Context->Geometry()->FrameToSceneX(m_Context->CurrentFrame()), 0.0);
    update();
}

void SPlayheadItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(QPen(option->palette.color(QPalette::Highlight), 2));
    painter->drawLine(QPoint(0, 0), QPoint(0, boundingRect().height()));
    // VOID_LOG_INFO("Height: {0}", boundingRect().height());
}

VOID_NAMESPACE_CLOSE
