// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Qt */
#include <QPainter>
#include <QPalette>
#include <QGraphicsSceneMouseEvent>

/* Internal */
#include "STimelineScene.h"
#include "Graphics/SPlayheadItem.h"
#include "Graphics/SRazorItem.h"
#include "Graphics/STrack.h"
#include "Graphics/STrackItem.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

// class TestItem : public QGraphicsRectItem
// {
// public:
//     TestItem()
//         : QGraphicsRectItem(0,0, 200, 200)
//     {
//         setBrush(Qt::red);
//     }
// };

STimelineScene::STimelineScene(SequencerContext* context, QObject* parent)
    : QGraphicsScene(parent)
    , m_Context(context)
    , m_Playhead(nullptr)
    , m_Razorhead(nullptr)
{
    // Can now be accessed directly by any other sub-component within the sequencer
    m_Context->Controller()->SetScene(this);

    setSceneRect(0, 0, Sequencer::SceneWidth, SceneHeight());
    connect(m_Context->Controller(), &SequencerController::frameChanged, this, &STimelineScene::UpdatePlayhead, Qt::DirectConnection);
}

void STimelineScene::AddTrack(const SharedPlaybackTrack& track)
{
    STrack* strack = new STrack(track, m_Context);
    m_Tracks.push_back(strack);
    addItem(strack);

    Update();
}

void STimelineScene::RemoveTrack(const SharedPlaybackTrack& track)
{
    auto _pred = [track](const STrack* t) -> bool { return track.get() == t->Track().get(); };
    auto it = std::find_if(m_Tracks.begin(), m_Tracks.end(), _pred);

    if (it == m_Tracks.end())
        return;

    STrack*& strack = *it;
    removeItem(strack);

    strack->deleteLater();
    delete strack;
    strack = nullptr;

    m_Tracks.erase(it);
}

void STimelineScene::Clear()
{
    for (auto& track : m_Tracks)
    {
        track->deleteLater();
        delete track;
        track = nullptr;
    }
    m_Tracks.clear();

    if (m_Playhead)
    {
        m_Playhead->deleteLater();
        delete m_Playhead;
        m_Playhead = nullptr;
    }

    if (m_Razorhead)
    {
        m_Razorhead->deleteLater();
        delete m_Razorhead;
        m_Razorhead = nullptr;
    }

    clear();
}

void STimelineScene::AddRazorhead()
{
    m_Razorhead = new SRazorItem(m_Context);
    addItem(m_Razorhead);
}

void STimelineScene::SetRazorX(int x)
{
    m_Razorhead->SetX(x);
}

void STimelineScene::ToggleRazorhead(bool visible)
{
    if (!m_Razorhead) AddRazorhead();
    m_Razorhead->setVisible(visible);
}

void STimelineScene::AddPlayhead()
{
    m_Playhead = new SPlayheadItem(m_Context);
    addItem(m_Playhead);
}

void STimelineScene::UpdatePlayhead()
{
    m_Playhead->Update();
}

void STimelineScene::Update()
{
    setSceneRect(0, 0, Sequencer::SceneWidth, SceneHeight());
}

void STimelineScene::UpdateItems()
{
    setSceneRect(0, 0, Sequencer::SceneWidth, SceneHeight());
    m_Playhead->Update();

    for (STrack* track : m_Tracks)
    {
        track->Update();
        track->UpdateItems();
    }
}

STrack* STimelineScene::TrackAt(int index) const
{
    return m_Tracks.at(index);
}

STrack*& STimelineScene::TrackAt(int index)
{
    return m_Tracks.at(index);
}

void STimelineScene::SelectTrackItems(const QRectF& rect)
{
    const QList<QGraphicsItem*> hits = items(rect, Qt::IntersectsItemShape);
    std::vector<SharedTrackItem> trackitems;
    trackitems.reserve(hits.size());
    for (auto& item : hits)
    {
        if (const auto& trackitem = dynamic_cast<STrackItem*>(item))
            trackitems.emplace_back(trackitem->TrackItem());
    }

    m_Context->SelectionModel()->Select(trackitems);
}

void STimelineScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, palette().color(QPalette::Base));
}

void STimelineScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if ((event->button() == Qt::LeftButton) && !(event->modifiers() & Qt::ControlModifier))
        m_Context->SelectionModel()->Clear();

    QGraphicsScene::mousePressEvent(event);
}

int STimelineScene::SceneHeight() const
{
    return Sequencer::RulerHeight + m_Tracks.size() * (Sequencer::TrackHeight + Sequencer::TrackSpacing);
}

VOID_NAMESPACE_CLOSE
