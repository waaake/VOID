// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "SHandleItem.h"

VOID_NAMESPACE_OPEN

SHandleItem::SHandleItem(const SharedTrackItem& item, const HandleType& type, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Item(item)
    , m_Type(type)
{
    CalculateBoundingRect();
}

void SHandleItem::Update()
{
    prepareGeometryChange();
    CalculateBoundingRect();
    update();
}

void SHandleItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(QPen(option->palette.color(QPalette::Highlight), 1));
    painter->drawLine(boundingRect().left(), Sequencer::TrackItemHeight - 20, boundingRect().right(), Sequencer::TrackItemHeight - 20);

    painter->drawLine(boundingRect().left(), Sequencer::TrackItemHeight - 24, boundingRect().left(), Sequencer::TrackItemHeight - 16);
    painter->drawLine(boundingRect().right(), Sequencer::TrackItemHeight - 24, boundingRect().right(), Sequencer::TrackItemHeight - 16);

    painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(
        m_Type == HandleType::HEAD ? m_Item->HeadHandle() : m_Item->TailHandle()
    ));
}

void SHandleItem::CalculateBoundingRect()
{
    if (m_Type == HandleType::HEAD)
    {
        int width = m_Context->Geometry()->FrameToSceneX(m_Item->HeadHandle());
        m_BoundingRect = QRectF(-width, 0, width, Sequencer::TrackItemHeight - 4);
    }
    else
    {
        m_BoundingRect = QRectF(
            0, 0, m_Context->Geometry()->FrameToSceneX(m_Item->TailHandle()), Sequencer::TrackItemHeight - 4
        );
    }
    
}

VOID_NAMESPACE_CLOSE
