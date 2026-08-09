// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QPalette>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "SRazorItem.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SRazorItem::SRazorItem(SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
{
    m_BoundingRect = QRectF(-1, 0, 2, Sequencer::SceneHeight);
    setZValue(Sequencer::ZPlayheadItem);
}

void SRazorItem::SetX(int x)
{
    prepareGeometryChange();
    setPos(x, 0.0);
    update();
}

void SRazorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(QPen(option->palette.color(QPalette::Highlight), 1));
    painter->drawLine(QPoint(0, 0), QPoint(0, boundingRect().height()));
}

void SRazorItem::SetHeight(int height)
{
    prepareGeometryChange();
    m_BoundingRect = QRectF(-1, 0, 2, height);
    update();    
}

/// STrackRazorItem

STrackRazorItem::STrackRazorItem(SequencerContext* context, QGraphicsItem* parent)
    : SRazorItem(context, parent)
{
    m_BoundingRect = QRectF(-1, 0, 2, Sequencer::TrackHeight);
}

VOID_NAMESPACE_CLOSE
