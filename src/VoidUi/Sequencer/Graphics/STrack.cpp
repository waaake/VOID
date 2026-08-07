// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "STrack.h"
#include "STrackItem.h"
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

    int index = track->TrackIndex();
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
    int index = m_Track->TrackIndex();
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
        m_RazorMarker->setVisible(true);
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
