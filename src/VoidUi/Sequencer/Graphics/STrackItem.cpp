// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "STrackItem.h"
#include "VoidUi/Sequencer/STimelineGeometry.h"

VOID_NAMESPACE_OPEN

STrackItem::STrackItem(const SharedTrackItem& item, STimelineGeometry* geometry, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_Item(item)
    , m_Geometry(geometry)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    CalculateBoundingRect();
    setPos(geometry->FrameToSceneX(item->TimelineIn()), 4);

    // connect(m_Item, &TrackItem::updated, this, &STrackItem::Update);
}

QRectF STrackItem::boundingRect() const
{
    return m_BoundingRect;
}

void STrackItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    const QColor color = isSelected() ? option->palette.color(QPalette::Highlight).darker(180) : m_Item->Color();

    painter->setPen(color.darker(200));
    painter->setBrush(color);
    // painter->fillRect(boundingRect(), m_Item->Color());
    painter->drawRect(boundingRect());

    painter->setPen(option->palette.color(QPalette::Text));
    painter->drawText(boundingRect().adjusted(8, 0, -8, 0), Qt::AlignLeft | Qt::AlignVCenter, m_Item->Name().c_str());
}

void STrackItem::Update()
{
    prepareGeometryChange();
    CalculateBoundingRect();
    setPos(m_Geometry->FrameToSceneX(m_Item->TimelineIn()), 4);

    update();
}

void STrackItem::CalculateBoundingRect()
{
    const double width = m_Geometry->FrameToSceneX(m_Item->Duration()) - m_Geometry->FrameToSceneX(0);
    m_BoundingRect = QRectF(0, 0, width, 52);
}

VOID_NAMESPACE_CLOSE
