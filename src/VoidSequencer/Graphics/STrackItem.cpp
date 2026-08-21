// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "STrackItem.h"
#include "STrack.h"
#include "STimelineEffect.h"
#include "VoidSequencer/SContext.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

STrackItem::STrackItem(const SharedTrackItem& item, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Item(item)
{
    setZValue(Sequencer::ZTrackItem);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    m_HeadHandle = new SHandleItem(m_Item->HeadHandle(), context);
    m_TailHandle = new SHandleItem(m_Item->TailHandle(), context);
    m_DurationHandle = new SHandleItem(m_Item->Duration(), context);
    m_Context->Controller()->AddToScene(m_HeadHandle);
    m_Context->Controller()->AddToScene(m_TailHandle);
    m_Context->Controller()->AddToScene(m_DurationHandle);

    ToggleHandles();

    CalculateBoundingRect();
    setPos(context->Geometry()->FrameToSceneX(item->TimelineIn()), YPos());

    connect(m_Context->SelectionModel(), &SSelectionModel::selectionChanged, this, [this]() { update(); });
    connect(m_Item.get(), &TrackItem::updated, this, &STrackItem::Update);
    connect(m_Item.get(), &TrackItem::rangeChanged, this, &STrackItem::Update);
    connect(m_Item.get(), &TrackItem::effectCreated, this, static_cast<void (STrackItem::*)(Effect*)>(&STrackItem::AddEffect));
    connect(m_Item.get(), &TrackItem::effectAboutToBeRemoved, this, &STrackItem::RemoveEffect);

    AddEffects();
}

STrackItem::~STrackItem()
{
    m_Context->Controller()->RemoveFromScene(m_HeadHandle);
    m_HeadHandle->deleteLater();
    delete m_HeadHandle;
    m_HeadHandle = nullptr;

    m_Context->Controller()->RemoveFromScene(m_TailHandle);
    m_TailHandle->deleteLater();
    delete m_TailHandle;
    m_TailHandle = nullptr;

    m_Context->Controller()->RemoveFromScene(m_DurationHandle);
    m_DurationHandle->deleteLater();
    delete m_DurationHandle;
    m_DurationHandle = nullptr;
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

        if (m_Context->Action() == SequencerAction::TRIM && m_Context->HoverModel()->IsHovered(m_Item) && !m_TrimContext.active)
        {
            painter->setPen(QPen(option->palette.color(QPalette::Highlight), 2));
            painter->drawLine(m_HeadTrimRect.right(), 8, m_HeadTrimRect.right(), m_HeadTrimRect.height() - 8);
            painter->drawLine(m_TailTrimRect.left(), 8, m_TailTrimRect.left(), m_TailTrimRect.height() - 8);
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
    setPos(m_Context->Geometry()->FrameToSceneX(m_Item->TimelineIn()), YPos());

    update();
}

void STrackItem::UpdateItems()
{
    for (auto& [_, effect] : m_Effects)
        effect->Update();
}

void STrackItem::AddEffect(Effect* effect)
{
    AddEffect(effect, m_Item->EffectIndex(effect));
}

void STrackItem::AddEffect(Effect* effect, int index)
{
    STimelineEffect* timelineEffect = new STimelineEffect(effect, m_Context, this);
    timelineEffect->setPos(0, -(Sequencer::TimelineEffectHeight + index * Sequencer::TimelineEffectHeight));

    m_Effects[effect] = timelineEffect;
}

void STrackItem::AddEffects()
{
    for (int i = 0; i < m_Item->NumEffects(); ++i)
        AddEffect(m_Item->EffectAt(i), i);
}

void STrackItem::RemoveEffect(Effect* effect)
{
    if (m_Effects.find(effect) == m_Effects.end()) return;

    STimelineEffect*& teffect = m_Effects[effect];
    teffect->setParent(nullptr);
    teffect->setVisible(false);
    teffect->deleteLater();
    delete teffect;
    teffect = nullptr;

    m_Effects.erase(effect);
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

            // Hide handles before we drag
            ToggleHandles();
        }
        else if (m_Context->Action() == SequencerAction::SLIP_CLIP)
        {
            m_SlipContext.pressed = true;
            m_SlipContext.sourcepos = event->scenePos();
        }
        else if (m_Context->Action() == SequencerAction::TRIM)
        {
            if (m_HeadTrimRect.contains(event->pos()))
            {
                m_TrimContext.sourcepos = event->scenePos();
                m_TrimContext.type = SItemTrimContext::HEAD;
                m_TrimContext.pressed = true;

                ToggleHandles();
            }
            else if (m_TailTrimRect.contains(event->pos()))
            {
                m_TrimContext.sourcepos = event->scenePos();
                m_TrimContext.type = SItemTrimContext::TAIL;
                m_TrimContext.pressed = true;

                ToggleHandles();
            }
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
        if (delta.manhattanLength() > Sequencer::DragTravelDistance)
        {
            m_Drag.pressed = false;
            m_Drag.active = true;
        }
    }

    if (m_SlipContext.pressed)
    {
        QPointF delta = event->scenePos() - m_SlipContext.sourcepos;
        if (delta.manhattanLength() > Sequencer::SlipTravelDistance)
        {
            m_SlipContext.pressed = false;
            m_SlipContext.active = true;
        }
    }

    if (m_TrimContext.pressed)
    {
        QPointF delta = event->scenePos() - m_TrimContext.sourcepos;
        if (delta.manhattanLength() > Sequencer::SlipTravelDistance)
        {
            m_TrimContext.pressed = false;
            m_TrimContext.active = true;
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

        // Dynamic handles shifting as we move
        ToggleHandles(m_Item->HeadHandle() + m_SlipContext.offset, m_Item->TailHandle() - m_SlipContext.offset, 0, true);
        update();
    }

    if (m_TrimContext.active)
    {
        AdjustTimelineRange(m_Context->Geometry()->SceneXToFrame(event->scenePos().x() - m_SlipContext.sourcepos.x()));
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

        if (track && (track->Locked() || track->IsEffectsTrack()))
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
        ToggleHandles(m_Item->HeadHandle(), m_Item->TailHandle(), m_Item->Duration(), true);
    }

    if (m_TrimContext.active)
    {
        m_TrimContext.type == SItemTrimContext::HEAD
            ? m_Context->Controller()->TrimItemHead(m_Item, m_TrimContext.handle)
            : m_Context->Controller()->TrimItemTail(m_Item, m_TrimContext.handle);

        m_TrimContext.active = false;
        m_TrimContext.handle = 0;
        ToggleHandles(m_Item->HeadHandle(), m_Item->TailHandle(), m_Item->Duration(), true);
    }
}

void STrackItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    if (m_Context->Action() < SequencerAction::RAZOR)
    {
        m_Context->HoverModel()->Set(m_Item);
        ToggleHandles(m_Item->HeadHandle(), m_Item->TailHandle(), m_Item->Duration(), true);
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
    if (m_Context->Action() < SequencerAction::RAZOR)
    {
        m_Context->HoverModel()->Reset();
        ToggleHandles();
        update();
    }

    STimelineItem::hoverLeaveEvent(event);
}

void STrackItem::CalculateBoundingRect()
{
    const double width = m_Context->Geometry()->FrameToSceneX(m_Item->TimelineOut() + 1) - m_Context->Geometry()->FrameToSceneX(m_Item->TimelineIn());
    m_BoundingRect = QRectF(0, 0, width, Sequencer::TrackItemHeight - 4);

    m_HeadTrimRect = QRectF(m_BoundingRect.topLeft(), QPointF(std::min(m_BoundingRect.topLeft().x() + 10, width), m_BoundingRect.height()));
    m_TailTrimRect = QRectF(QPointF(std::max(m_BoundingRect.topRight().x() - 10, m_BoundingRect.left()), m_BoundingRect.top()), m_BoundingRect.bottomRight());
}

void STrackItem::AdjustTimelineRange(v_frame_t frame)
{
    prepareGeometryChange();

    double width = 0;
    int head = m_Item->HeadHandle();
    int tail = m_Item->TailHandle();
    if (m_TrimContext.type == SItemTrimContext::HEAD)
    {
        frame = std::min(std::max(frame, m_Item->TimelineIn() - m_Item->HeadHandle()), m_Item->TimelineOut());
        width = m_Context->Geometry()->FrameToSceneX(m_Item->TimelineOut() + 1) - m_Context->Geometry()->FrameToSceneX(frame);
        setPos(m_Context->Geometry()->FrameToSceneX(frame), YPos());

        m_TrimContext.handle = frame - m_Item->TimelineIn();
        head += m_TrimContext.handle;
    }
    else
    {
        frame = std::max(std::min(frame, m_Item->TimelineOut() + m_Item->TailHandle()), m_Item->TimelineIn());
        width = m_Context->Geometry()->FrameToSceneX(frame + 1) - m_Context->Geometry()->FrameToSceneX(m_Item->TimelineIn());
        setPos(m_Context->Geometry()->FrameToSceneX(m_Item->TimelineIn()), YPos());

        m_TrimContext.handle = m_Item->TimelineOut() - frame;
        tail += m_TrimContext.handle;
    }

    m_BoundingRect = QRectF(0, 0, width, Sequencer::TrackItemHeight - 4);
    // Need to see how expensive this becomes, we don't want slowness while dragging -- so keeping an eye on performance
    AdjustEffectsWidth(width);

    // Dynamic handles shifting as we move
    ToggleHandles(head, tail, m_Item->Duration() - m_TrimContext.handle, true);
    update();
}

void STrackItem::AdjustEffectsWidth(double width)
{
    for (auto& [_, teffect] : m_Effects)
        teffect->SetWidth(width);
}

QColor STrackItem::Background(const QStyleOptionGraphicsItem* option) const
{
    if (Track()->Locked()) return option->palette.color(QPalette::Base).darker(150);
    if (m_Context->SelectionModel()->IsSelected(m_Item)) return option->palette.color(QPalette::Highlight).darker(180);

    if (Track()->Enabled() && m_Item->Enabled())
    {
        QColor color = m_Item->Color().darker(250);
        return m_Context->HoverModel()->IsHovered(m_Item) ? color.darker(140) : color;
    }

    // Disabled
    return option->palette.color(QPalette::Base).darker(180);
}

int STrackItem::YPos() const
{
    return Track()->boundingRect().height() - Sequencer::TrackItemHeight + 2;
}

void STrackItem::ToggleHandles(int head, int tail, int duration, bool visible)
{
    m_HeadHandle->setVisible(visible);
    m_TailHandle->setVisible(visible);
    m_DurationHandle->setVisible(visible);
    if (visible)
    {
        // This results in a matrix mult -- need to see if there is a better way to do this
        m_HeadHandle->setPos(mapToScene(0 - m_Context->Geometry()->FrameToSceneX(head), 22));
        m_TailHandle->setPos(mapToScene(boundingRect().width(), 22));
        m_DurationHandle->setPos(mapToScene(0, 22));
        m_HeadHandle->Update(head);
        m_TailHandle->Update(tail);
        m_DurationHandle->Update(duration);
    }
}

VOID_NAMESPACE_CLOSE
