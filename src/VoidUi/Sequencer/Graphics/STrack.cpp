// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "STrack.h"
#include "STrackItem.h"
#include "VoidUi/Sequencer/STimelineGeometry.h"

VOID_NAMESPACE_OPEN

STrack::STrack(const SharedPlaybackTrack& track, int index, STimelineGeometry* geometry, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_Track(track)
    , m_Geometry(geometry)
    , m_Index(index)
{
    m_BoundingRect = QRectF(0, 0, Sequencer::SceneWidth, Sequencer::TrackHeight);
    setPos(0, geometry->TrackRect(index).top());
    BuildItems();
}

STrack::~STrack()
{
    Clear();
}

QRectF STrack::boundingRect() const
{
    return m_BoundingRect;
}

void STrack::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // QColor bg = (m_Index % 2) ? option->palette.color(QPalette::Base) : option->palette.color(QPalette::AlternateBase);

    painter->fillRect(boundingRect(), option->palette.color(QPalette::AlternateBase));
    // painter->fillRect(QRectF(0, 0, Sequencer::TrackHeaderWidth, boundingRect().height()), option->palette.color(QPalette::Dark));

    // painter->setPen(QColor(80, 80, 80));
    // painter->drawLine(boundingRect().bottomLeft(), boundingRect().bottomRight());

    // painter->setPen(option->palette.color(QPalette::Text));
    // painter->drawText(QRectF(10, 0, 100, boundingRect().height()), Qt::AlignVCenter, m_Track->Name().c_str());
}

void STrack::Update()
{
    prepareGeometryChange();
    // m_BoundingRect = QRectF(0, 0, Sequencer::SceneWidth, Sequencer::TrackHeight);
    setPos(0, m_Geometry->TrackRect(m_Index).top());

    update();
}

void STrack::Clear()
{
    // for (auto& item : m_Items)
    // {
    //     delete item;
    //     item = nullptr;
    // }
    for (auto& [_, item] : m_Items)
    {
        delete item;
        item = nullptr;
    }

    m_Items.clear();
}

void STrack::AddItem(const SharedTrackItem& item)
{
    STrackItem* trackitem = new STrackItem(item, m_Geometry, this);
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
    // STrackItem* trackitem = m_Items[item.get()];
    // trackitem->Update();
    // auto it = m_Items.find(item.get());
    // if (it == m_Items.end())
    //     return;

    // it->
    if (m_Items.find(item.get()) == m_Items.end())
        return;

    m_Items[item.get()]->Update();    
}

void STrack::BuildItems()
{
    // for (auto& item : m_Track->It)
    // for (int i = 0; i < m_Track->ItemCount(); ++i)
    //     new STrackItem(m_Track->ItemAt(i), m_Geometry, this);

    m_Items.reserve(m_Track->ItemCount());
    for (int i = 0; i < m_Track->ItemCount(); ++i)
        AddItem(m_Track->ItemAt(i));

    // {
    //     STrackItem* item = new STrackItem(m_Track->ItemAt(i), m_Geometry, this);
    //     // m_Items.push_back(item);
    //     m_Items[m_Track->ItemAt(i).get()] = item;
    // }
}

VOID_NAMESPACE_CLOSE
