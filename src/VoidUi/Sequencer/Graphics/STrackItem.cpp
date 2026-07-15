// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "STrackItem.h"
#include "STrack.h"
#include "VoidUi/Sequencer/SContext.h"
#include "VoidCore/Logging.h"
// #include "SDrawContext.h"

VOID_NAMESPACE_OPEN

STrackItem::STrackItem(const SharedTrackItem& item, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Item(item)
{
    setZValue(Sequencer::ZTrackItem);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    CalculateBoundingRect();
    setPos(context->Geometry()->FrameToSceneX(item->TimelineIn()), 2);

    // connect(m_Context->SelectionModel(), &SSelectionModel::selectionChanged, this, static_cast<void (STrackItem::*)(const QRectF&)>(&STrackItem::update));
    connect(m_Context->SelectionModel(), &SSelectionModel::selectionChanged, this, [this]() { update(); });
    connect(m_Item.get(), &TrackItem::updated, this, &STrackItem::Update);
}

// QRectF STrackItem::boundingRect() const
// {
//     return m_BoundingRect;
// }

STrack* STrackItem::Track() const
{
    return dynamic_cast<STrack*>(parentItem());
}

void STrackItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHint(QPainter::Antialiasing);

    const QColor color = m_Context->SelectionModel()->IsSelected(m_Item)
                        ? option->palette.color(QPalette::Highlight).darker(180)
                        : m_Context->HoverModel()->IsHovered(m_Item)
                            ? option->palette.color(QPalette::Base).darker(140)
                            : option->palette.color(QPalette::Base).darker(110);

    painter->setPen(QPen(m_Item->Color(), 1));
    painter->setBrush(color);
    painter->drawRect(boundingRect());

    painter->fillRect(2, 2, 6, Sequencer::TrackHeight - 8, m_Item->Color());

    painter->setPen(option->palette.color(QPalette::Text));
    painter->drawText(boundingRect().adjusted(10, 0, -2, 0), Qt::AlignLeft | Qt::AlignTop, m_Item->Name().c_str());

    if (SharedMediaClip media = m_Item->GetMedia())
    {
        QPixmap thumbnail = media->Thumbnail();
        QRectF thumbRect(10, 16, std::min(72, option->rect.width() - 10), 36);

        QSizeF size = thumbnail.size();
        size.scale(thumbRect.size(), Qt::KeepAspectRatio);

        QRectF drawRect(thumbRect.x(), thumbRect.y(), size.width(), size.height());
        drawRect.moveCenter(thumbRect.center());

        painter->drawPixmap(drawRect, thumbnail, thumbnail.rect());
    }

}

void STrackItem::Update()
{
    prepareGeometryChange();
    CalculateBoundingRect();
    setPos(m_Context->Geometry()->FrameToSceneX(m_Item->TimelineIn()), 0);

    update();
}

void STrackItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
        m_Context->SelectionModel()->Toggle(m_Item);
    else
        m_Context->SelectionModel()->Select(m_Item);

    if (event->button() == Qt::LeftButton)
    {
        m_Drag.pressed = true;
        m_Drag.clickpos = event->scenePos();
        m_Drag.scenepos = scenePos();
        m_Drag.offset = event->pos();
    }

    event->accept();
    STimelineItem::mousePressEvent(event);
}

void STrackItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_Drag.pressed)
    {
        QPointF delta = event->scenePos() - m_Drag.clickpos;
        if (delta.manhattanLength() > 10)
        {
            m_Drag.pressed = false;
            m_Drag.active = true;
        }
    }

    if (m_Drag.active)
    {
        QPointF pos = event->scenePos() - m_Drag.offset;
        if (event->modifiers() & Qt::ControlModifier)
        {
            if (STrack* track = m_Context->Controller()->TrackAt(scenePos()))
            {
                auto snap = track->Track()->GetSnapFrame(m_Context->Geometry()->SceneXToFrame(scenePos().x()), m_Item);
                if (snap > 0)
                    pos.setX(m_Context->Geometry()->FrameToSceneX(snap));
            }
        }

        setPos(pos);
    }

    STimelineItem::mouseMoveEvent(event);
}

void STrackItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_Drag.active)
    {
        m_Drag.active = false;

        v_frame_t frame = m_Context->Geometry()->SceneXToFrame(scenePos().x());
        STrack* track = m_Context->Controller()->TrackAt(scenePos());
        STrack* current = Track();

        // Move the track item to the new track
        if (track && track != current)
        {
            m_Context->Controller()->MoveItem(m_Item, current->Index(), track->Index(), frame);
        }
        else
        {
            m_Context->Controller()->MoveItem(m_Item, frame);
            Update();
        }
    }

    STimelineItem::mouseMoveEvent(event);
}

void STrackItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    m_Context->HoverModel()->Set(m_Item);
    update();

    STimelineItem::hoverEnterEvent(event);
}

void STrackItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    m_Context->HoverModel()->Reset();
    update();

    STimelineItem::hoverLeaveEvent(event);
}

void STrackItem::CalculateBoundingRect()
{
    const double width = m_Context->Geometry()->FrameToSceneX(m_Item->Duration()) - m_Context->Geometry()->FrameToSceneX(0);
    m_BoundingRect = QRectF(0, 0, width, Sequencer::TrackHeight - 4);
}

VOID_NAMESPACE_CLOSE
