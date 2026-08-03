// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "SHandleItem.h"

VOID_NAMESPACE_OPEN

SHandleItem::SHandleItem(int handle, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Handle(handle)
{
    setZValue(Sequencer::ZMarker);
    CalculateBoundingRect();
}

void SHandleItem::Update()
{
    prepareGeometryChange();
    CalculateBoundingRect();
    update();
}

void SHandleItem::Update(int handle)
{
    m_Handle = handle;
    prepareGeometryChange();
    CalculateBoundingRect();
    update();
}

void SHandleItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (m_Handle)
    {
        painter->setPen(QPen(option->palette.color(QPalette::Highlight), 1));
        painter->drawLine(boundingRect().left(), Sequencer::TrackItemHeight - 20, boundingRect().right(), Sequencer::TrackItemHeight - 20);
    
        painter->drawLine(boundingRect().left(), Sequencer::TrackItemHeight - 24, boundingRect().left(), Sequencer::TrackItemHeight - 16);
        painter->drawLine(boundingRect().right(), Sequencer::TrackItemHeight - 24, boundingRect().right(), Sequencer::TrackItemHeight - 16);
    
        painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(m_Handle));
    }
}

void SHandleItem::CalculateBoundingRect()
{
    m_BoundingRect = QRectF(0, 0, m_Context->Geometry()->FrameToSceneX(m_Handle), Sequencer::TrackItemHeight - 4);
}

VOID_NAMESPACE_CLOSE
