// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "STrack.h"
#include "STrackItem.h"
#include "STimelineEffect.h"
#include "VoidUi/Sequencer/SContext.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

STrack::STrack(const SharedPlaybackTrack& track, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Track(track)
{
    setAcceptHoverEvents(true);

    setZValue(Sequencer::ZTrack);
    connect(m_Track.get(), &PlaybackTrack::updated, this, &STrack::UpdateItems);
    connect(m_Track.get(), &PlaybackTrack::itemAdded, this, &STrack::AddItem);
    connect(m_Track.get(), &PlaybackTrack::cleared, this, &STrack::Clear);
    connect(m_Track.get(), &PlaybackTrack::itemAboutToBeRemoved, this, &STrack::RemoveItem);
    connect(m_Track.get(), &PlaybackTrack::itemRemoved, this, &STrack::Update);
    connect(m_Track.get(), &PlaybackTrack::effectAdded, this, [this](Effect* effect) -> void
    {
        if (effect->GetEffectType() == Effect::EffectType::TRACK)
            AddEffect(effect, m_Track->EffectIndex(effect));
    });
    connect(m_Track.get(), &PlaybackTrack::effectAboutToBeRemoved, this, [this](Effect* effect) -> void
    {
        if (effect->GetEffectType() == Effect::EffectType::TRACK)
            RemoveEffect(effect);
    });

    int index = track->Index();
    m_BoundingRect = QRectF(
        0,
        0,
        Sequencer::SceneWidth,
        m_Track->Type() == Sequence::TrackType::VIDEO
            ? m_Context->Geometry()->VideoTrackHeight(index)
            : m_Context->Geometry()->AudioTrackHeight(index)
    );
    setPos(0, context->Geometry()->TrackRect(index).top());

    BuildItems();
    AddEffects();

    m_RazorMarker = new STrackRazorItem(context, this);
    m_RazorMarker->setVisible(false);
}

STrack::~STrack()
{
    Clear();
    VOID_LOG_INFO("STrack::Destructor");
}

// STrackItem* STrack::BuildItem(int index)
// {
//     if (auto item = m_Track->ItemAt(index))
//     {
//         AddItem(item);
//         return m_Items[item.get()];
//     }
//     return nullptr;
// }

void STrack::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->fillRect(boundingRect(), option->palette.color(QPalette::AlternateBase));
}

void STrack::Update()
{
    prepareGeometryChange();
    int index = m_Track->Index();
    setPos(0, m_Context->Geometry()->TrackRect(index).top());
    
    m_BoundingRect = QRectF(
        0,
        0,
        Sequencer::SceneWidth,
        m_Track->Type() == Sequence::TrackType::VIDEO
            ? m_Context->Geometry()->VideoTrackHeight(index)
            : m_Context->Geometry()->AudioTrackHeight(index)
    );

    update();
}

void STrack::Clear()
{
    for (auto& [_, item] : m_Items)
    {
        item->deleteLater();
        delete item;
        item = nullptr;
    }

    m_Items.clear();
}

void STrack::AddEffect(Effect* effect)
{
    AddEffect(effect, m_Track->EffectIndex(effect));
}

void STrack::AddEffect(Effect* effect, int index)
{
    STimelineEffect* teffect = new STimelineEffect(effect, m_Context, this);
    teffect->setPos(0, boundingRect().height() - 2 - (Sequencer::TimelineEffectHeight + Sequencer::TimelineEffectHeight * index));
    m_Effects[effect] = std::move(teffect);
}

void STrack::AddEffects()
{
    for (int i = 0; i < m_Track->NumEffects(); ++i)
        AddEffect(m_Track->EffectAt(i), i);
}

void STrack::RemoveEffect(Effect* effect)
{
    if (m_Effects.find(effect) == m_Effects.end())
        return;
    
    STimelineEffect*& teffect = m_Effects[effect];
    teffect->setVisible(false);
    teffect->setParent(nullptr);
    teffect->deleteLater();
    delete teffect;
    teffect = nullptr;

    m_Effects.erase(effect);
}

void STrack::AddItem(const SharedTrackItem& item)
{
    STrackItem* trackitem = new STrackItem(item, m_Context, this);
    m_Items[item.get()] = std::move(trackitem);
}

void STrack::RemoveItem(const SharedTrackItem& item)
{
    if (m_Items.find(item.get()) == m_Items.end())
        return;

    STrackItem*& trackitem = m_Items[item.get()];
    trackitem->setVisible(false);
    trackitem->setParent(nullptr);
    trackitem->deleteLater();
    delete trackitem;
    trackitem = nullptr;

    m_Items.erase(item.get());
}

void STrack::UpdateEffects()
{
    for (auto& [effect, teffect] : m_Effects)
    {
        teffect->setPos(0, boundingRect().height() - 2 - (Sequencer::TimelineEffectHeight + Sequencer::TimelineEffectHeight * m_Track->EffectIndex(effect)));
        teffect->Update();
    }
}

void STrack::UpdateItem(const SharedTrackItem& item)
{
    if (m_Items.find(item.get()) == m_Items.end())
        return;

    m_Items[item.get()]->Update();
}

void STrack::UpdateItems()
{
    for (auto& [_, trackitem] : m_Items)
    {
        trackitem->Update();
        trackitem->UpdateItems();
    }
}

STrackItem* STrack::ItemAt(int index) const
{
    if (SharedTrackItem item = m_Track->ItemAt(index))
    {
        if (m_Items.find(item.get()) == m_Items.end())
            return nullptr;

        return m_Items.at(item.get());
    }

    return nullptr;
}

STrackItem* STrack::ItemAt(int index)
{
    if (SharedTrackItem item = m_Track->ItemAt(index))
    {
        if (m_Items.find(item.get()) == m_Items.end())
            return nullptr;

        return m_Items.at(item.get());
    }

    return nullptr;
}

STrackItem* STrack::Item(const SharedTrackItem& item)
{
    if (m_Items.find(item.get()) == m_Items.end())
        return nullptr;

    return m_Items.at(item.get());
}

void STrack::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    STimelineItem::hoverEnterEvent(event);

    if (m_Context->Action() == SequencerAction::RAZOR)
    {
        m_RazorMarker->setVisible(true);
        m_RazorMarker->SetHeight(boundingRect().height());
    }
}

void STrack::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if (m_Context->Action() == SequencerAction::RAZOR)
        m_RazorMarker->SetX(event->scenePos().x());

    STimelineItem::hoverMoveEvent(event);
}

void STrack::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    STimelineItem::hoverLeaveEvent(event);

    if (m_Context->Action() == SequencerAction::RAZOR)
        m_RazorMarker->setVisible(false);
}

void STrack::BuildItems()
{
    m_Items.reserve(m_Track->NumItems());
    for (int i = 0; i < m_Track->NumItems(); ++i)
        AddItem(m_Track->ItemAt(i));
}

VOID_NAMESPACE_CLOSE
