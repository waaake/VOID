// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QPalette>
#include <QGraphicsSceneMouseEvent>

/* Internal */
#include "STimelineScene.h"
#include "Graphics/STrack.h"
#include "Graphics/SPlayheadItem.h"
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
{
    // Can now be accessed directly by any other sub-component within the sequencer
    m_Context->Controller()->SetScene(this);
    setSceneRect(0, 0, Sequencer::SceneWidth, Sequencer::SceneHeight);
    // m_Playhead = new SPlayheadItem(m_Context);
}

void STimelineScene::SetSequence(const SharedPlaybackSequence& sequence)
{
    clear();
    if (sequence->IsEmpty())
    {
        VOID_LOG_INFO("Sequence is Empty");
        return;
    }

    m_Playhead = new SPlayheadItem(m_Context);

    m_Tracks.resize(sequence->NumVideoTracks());
    for (int i = 0; i < sequence->NumVideoTracks(); ++i)
    {
        STrack* track = new STrack(sequence->VideoTrackAt(i), i, m_Context);
        m_Tracks[i] = track;
        addItem(track);
    }
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
    clear();

    m_Playhead->deleteLater();
    delete m_Playhead;
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

STrack* STimelineScene::TrackAt(int index) const
{
    return m_Tracks.at(index);
}

STrack*& STimelineScene::TrackAt(int index)
{
    return m_Tracks.at(index);
}

void STimelineScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, palette().color(QPalette::Base));
}

void STimelineScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!(event->modifiers() & Qt::ControlModifier))
        m_Context->SelectionModel()->Clear();

    QGraphicsScene::mousePressEvent(event);
}

VOID_NAMESPACE_CLOSE
