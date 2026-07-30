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

VOID_NAMESPACE_OPEN

STrackItem::STrackItem(const SharedTrackItem& item, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Item(item)
{
    setZValue(Sequencer::ZTrackItem);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    m_HeadHandle = new SHandleItem(m_Item, HandleType::HEAD, context, this);
    m_TailHandle = new SHandleItem(m_Item, HandleType::TAIL, context, this);
    ToggleHandles(false);

    CalculateBoundingRect();
    setPos(context->Geometry()->FrameToSceneX(item->TimelineIn()), 2);

    connect(m_Context->SelectionModel(), &SSelectionModel::selectionChanged, this, [this]() { update(); });
    connect(m_Item.get(), &TrackItem::updated, this, &STrackItem::Update);
    connect(m_Item.get(), &TrackItem::rangeChanged, this, &STrackItem::Update);
}

STrack* STrackItem::Track() const
{
    return dynamic_cast<STrack*>(parentItem());
}

void STrackItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    const int width = boundingRect().width();
    if (m_Item->Linked())
    {
        const QColor itemcol = Track()->Enabled() ? m_Item->Color() : m_Item->Color().darker(150);

        painter->setPen(QPen(itemcol, 1));
        painter->setBrush(Background(option));
        painter->drawRect(boundingRect());

        painter->fillRect(2, 2, std::min(6, width - 2) , Sequencer::TrackItemHeight - 8, itemcol);

        painter->setPen(option->palette.color(QPalette::Text));
        painter->drawText(boundingRect().adjusted(10, 0, -2, 0), Qt::AlignLeft | Qt::AlignTop, m_Item->Name().c_str());

        if (width < 40)
            return;

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

        if (m_SlipContext.active)
        {
            painter->setPen(option->palette.color(QPalette::Highlight));
            painter->drawText(
                boundingRect(),
                Qt::AlignCenter,
                m_SlipContext.offset > 0 ? QString("+%1").arg(m_SlipContext.offset) : (QString::number(m_SlipContext.offset))
            );
        }
    }
    else
    {
        QColor itemcol(160, 70, 50);
        painter->setPen(QPen(itemcol.darker(200), 1));
        painter->setBrush(m_Context->SelectionModel()->IsSelected(m_Item) ? option->palette.color(QPalette::Highlight).darker(180) : itemcol);
        painter->drawRect(boundingRect());

        const QRect trect(6, Sequencer::TrackItemHeight - 30, 40, 20);
        if (width > trect.right())
        {
            painter->fillRect(trect, itemcol.darker(180));

            painter->setPen(option->palette.color(QPalette::Text));
            painter->drawText(trect, Qt::AlignCenter, "OFF");
            painter->drawText(boundingRect().adjusted(10, 0, -2, 0), Qt::AlignLeft | Qt::AlignTop, m_Item->Name().c_str());
        }
    }

    painter->setPen(QPen(option->palette.color(QPalette::Highlight), 2));
    if (Track()->IsRazored(m_Item->TimelineIn()))
        painter->drawLine(boundingRect().left() + 4, 4, boundingRect().left() + 4, boundingRect().bottom() - 4);
    if (Track()->IsRazored(m_Item->TimelineOut()))
        painter->drawLine(boundingRect().right() - 4, 4, boundingRect().right() - 4, boundingRect().bottom() - 4);
}

void STrackItem::Update()
{
    prepareGeometryChange();
    CalculateBoundingRect();
    setPos(m_Context->Geometry()->FrameToSceneX(m_Item->TimelineIn()), 2);

    update();
}

void STrackItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !Track()->Locked())
    {
        if (m_Context->Action() == SequencerAction::NONE)
        {
            if (event->modifiers() & Qt::ControlModifier)
                m_Context->SelectionModel()->Toggle(m_Item);
            else
                m_Context->SelectionModel()->Select(m_Item);

            m_Drag.pressed = true;
            m_Drag.clickpos = event->scenePos();
            m_Drag.scenepos = scenePos();
            m_Drag.offset = event->pos();
        }
        else if (m_Context->Action() == SequencerAction::SLIP_CLIP)
        {
            m_SlipContext.pressed = true;
            m_SlipContext.sourcepos = event->scenePos();
        }

        event->accept();
    }

    if (m_Context->Action() == SequencerAction::RAZOR)
        m_Context->Controller()->RazorAt(Track()->Track(), m_Context->Geometry()->SceneXToFrame(event->scenePos().x()));
    else if (m_Context->Action() == SequencerAction::MERGE)
        m_Context->Controller()->MergeCut(Track()->Track(), m_Context->Geometry()->SceneXToFrame(event->scenePos().x()));

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

    if (m_SlipContext.pressed)
    {
        QPointF delta = event->scenePos() - m_SlipContext.sourcepos;
        if (delta.manhattanLength() > 10)
        {
            m_SlipContext.pressed = false;
            m_SlipContext.active = true;
        }
    }

    if (m_Drag.active)
    {
        QPointF pos = mapToParent(event->pos() - m_Drag.offset);
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

    if (m_SlipContext.active)
    {
        v_frame_t frame = m_Context->Geometry()->SceneXToFrame(event->scenePos().x() - m_SlipContext.sourcepos.x());
        m_SlipContext.offset = frame < 0 ? std::max(frame, -m_Item->HeadHandle()) : std::min(frame, m_Item->TailHandle());

        update();
    }

    STimelineItem::mouseMoveEvent(event);
}

void STrackItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    STimelineItem::mouseReleaseEvent(event);

    if (m_Drag.active)
    {
        m_Drag.active = false;

        v_frame_t frame = m_Context->Geometry()->SceneXToFrame(scenePos().x());
        STrack* track = m_Context->Controller()->TrackAt(scenePos());
        STrack* current = Track();

        if (track && track->Locked())
        {
            Update();
            return;
        }

        // Move the track item to the new track
        if (track && track != current)
        {
            m_Context->Controller()->MoveItem(current->Track(), m_Item, track->Index(), frame);
        }
        else
        {
            m_Context->Controller()->MoveItem(m_Item, frame);
            Update();
        }
    }

    if (m_SlipContext.active)
    {
        m_Context->Controller()->OffsetItemSource(m_Item, m_SlipContext.offset);

        m_SlipContext.active = false;
        m_SlipContext.offset = 0;
        ToggleHandles(true);
    }
}

void STrackItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    if (m_Context->Action() == SequencerAction::NONE || m_Context->Action() == SequencerAction::SLIP_CLIP)
    {
        m_Context->HoverModel()->Set(m_Item);
        ToggleHandles(true);
        update();
    }

    STimelineItem::hoverEnterEvent(event);
}

void STrackItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if (m_Context->Action() == SequencerAction::RAZOR)
        Track()->hoverMoveEvent(event);

    STimelineItem::hoverMoveEvent(event);
}

void STrackItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    if (m_Context->Action() == SequencerAction::NONE || m_Context->Action() == SequencerAction::SLIP_CLIP)
    {
        m_Context->HoverModel()->Reset();
        ToggleHandles(false);
        update();
    }

    STimelineItem::hoverLeaveEvent(event);
}

void STrackItem::CalculateBoundingRect()
{
    const double width = m_Context->Geometry()->FrameToSceneX(m_Item->Duration()) - m_Context->Geometry()->FrameToSceneX(0);
    m_BoundingRect = QRectF(0, 0, width, Sequencer::TrackItemHeight - 4);
}

QColor STrackItem::Background(const QStyleOptionGraphicsItem* option) const
{
    if (Track()->Locked()) return option->palette.color(QPalette::Base).darker(150);

    if (Track()->Enabled())
    {
        return m_Context->SelectionModel()->IsSelected(m_Item)
            ? option->palette.color(QPalette::Highlight).darker(180)
            : m_Context->HoverModel()->IsHovered(m_Item)
                ? option->palette.color(QPalette::Base).darker(140)
                : option->palette.color(QPalette::Base).darker(110);
    }

    return m_Context->SelectionModel()->IsSelected(m_Item)
            ? option->palette.color(QPalette::Highlight).darker(180)
            : option->palette.color(QPalette::Base).darker(180);
}

void STrackItem::ToggleHandles(bool visible)
{
    m_HeadHandle->setVisible(visible);
    m_TailHandle->setVisible(visible);
    if (visible)
    {
        m_HeadHandle->setPos(0, 2);
        m_TailHandle->setPos(boundingRect().width(), 2);
        m_HeadHandle->Update();
        m_TailHandle->Update();
    }
}

VOID_NAMESPACE_CLOSE
