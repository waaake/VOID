// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Qt */
#include <QPainter>
#include <QPalette>
#include <QGraphicsSceneMouseEvent>

/* Internal */
#include "SHeaderScene.h"
#include "STrackHeader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SHeaderScene::SHeaderScene(SequencerContext* context, QObject* parent)
    : QGraphicsScene(parent)
    , m_Context(context)
{
    ResizeScene();
}

SHeaderScene::~SHeaderScene()
{
    Clear();
}

void SHeaderScene::AddVideoTrack(const SharedPlaybackTrack& track)
{
    STrackHeaderItem* strack = new STrackHeaderItem(track, m_Context);
    m_VTracks.push_back(strack);
    addItem(strack);

    ResizeScene();
}

void SHeaderScene::AddAudioTrack(const SharedPlaybackTrack& track)
{
    STrackHeaderItem* strack = new STrackHeaderItem(track, m_Context);
    m_ATracks.push_back(strack);
    addItem(strack);

    ResizeScene();
}

void SHeaderScene::RemoveTrack(const SharedPlaybackTrack& track)
{
    track->Type() == Sequence::TrackType::VIDEO ? RemoveVideoTrack(track) : RemoveAudioTrack(track);
}

void SHeaderScene::Clear()
{
    for (auto& track : m_VTracks)
    {
        track->deleteLater();
        delete track;
        track = nullptr;
    }

    for (auto& track : m_ATracks)
    {
        track->deleteLater();
        delete track;
        track = nullptr;
    }

    m_VTracks.clear();
    m_ATracks.clear();
    clear();
}

void SHeaderScene::Update()
{
    ResizeScene();
}

void SHeaderScene::UpdateItems()
{
    for (STrackHeaderItem* track : m_VTracks)
        track->Update();

    for (STrackHeaderItem* track : m_ATracks)
        track->Update();
}

STrackHeaderItem* SHeaderScene::TrackAt(int index) const
{
    return m_VTracks.at(index);
}

STrackHeaderItem*& SHeaderScene::TrackAt(int index)
{
    return m_VTracks.at(index);
}

void SHeaderScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, palette().color(QPalette::Base));
}

void SHeaderScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if ((event->button() == Qt::LeftButton) && !(event->modifiers() & Qt::ControlModifier))
        m_Context->SelectionModel()->Clear();

    QGraphicsScene::mousePressEvent(event);
}

void SHeaderScene::RemoveVideoTrack(const SharedPlaybackTrack& track)
{
    auto _pred = [track](const STrackHeaderItem* t) -> bool { return track.get() == t->Track().get(); };
    auto it = std::find_if(m_VTracks.begin(), m_VTracks.end(), _pred);
    if (it == m_VTracks.end())
        return;

    STrackHeaderItem*& strack = *it;
    removeItem(strack);

    strack->deleteLater();
    delete strack;
    strack = nullptr;

    m_VTracks.erase(it);
}

void SHeaderScene::RemoveAudioTrack(const SharedPlaybackTrack& track)
{
    auto _pred = [track](const STrackHeaderItem* t) -> bool { return track.get() == t->Track().get(); };
    auto it = std::find_if(m_ATracks.begin(), m_ATracks.end(), _pred);
    if (it == m_ATracks.end())
        return;

    STrackHeaderItem*& strack = *it;
    removeItem(strack);

    strack->deleteLater();
    delete strack;
    strack = nullptr;

    m_ATracks.erase(it);
}

void SHeaderScene::ResizeScene()
{
    const auto* geo = m_Context->Geometry();
    int vh = geo->VideoSectionHeight();
    int ah = geo->AudioSectionHeight();
    setSceneRect(0, -vh, Sequencer::TrackHeaderWidth, vh + ah);
}

VOID_NAMESPACE_CLOSE
