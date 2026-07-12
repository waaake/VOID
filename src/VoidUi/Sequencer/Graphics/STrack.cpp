// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "STrack.h"
#include "STrackItem.h"
#include "VoidUi/Sequencer/SContext.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

STrack::STrack(const SharedPlaybackTrack& track, int index, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Track(track)
    , m_Index(index)
{
    VOID_LOG_INFO("Current Index::::::::{0}", index);
    setZValue(Sequencer::ZTrack);

    m_BoundingRect = QRectF(0, 0, Sequencer::SceneWidth, Sequencer::TrackHeight);
    setPos(0, context->Geometry()->TrackRect(index).top());
    BuildItems();
}

STrack::~STrack()
{
    Clear();
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
    // m_BoundingRect = QRectF(0, 0, Sequencer::SceneWidth, Sequencer::TrackHeight);
    setPos(0, m_Context->Geometry()->TrackRect(m_Index).top());

    update();
}

void STrack::Clear()
{
    for (auto& [_, item] : m_Items)
    {
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

void STrack::BuildItems()
{
    m_Items.reserve(m_Track->ItemCount());
    for (int i = 0; i < m_Track->ItemCount(); ++i)
        AddItem(m_Track->ItemAt(i));
}

VOID_NAMESPACE_CLOSE
