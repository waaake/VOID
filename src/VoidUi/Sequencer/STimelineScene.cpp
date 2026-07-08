// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QPalette>

/* Internal */
#include "STimelineScene.h"
#include "Graphics/STrack.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

STimelineScene::STimelineScene(STimelineGeometry* geometry, QObject* parent)
    : QGraphicsScene(parent)
    , m_Geometry(geometry)
{
    setSceneRect(0, 0, Sequencer::SceneWidth, Sequencer::SceneHeight);
}

void STimelineScene::SetSequence(const SharedPlaybackSequence& sequence)
{
    clear();
    if (sequence->IsEmpty())
    {
        VOID_LOG_INFO("Sequence is Empty");
        return;
    }

    for (int i = 0; i < sequence->NumVideoTracks(); ++i)
        addItem(new STrack(sequence->VideoTrackAt(i), i, m_Geometry));
}

void STimelineScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, palette().color(QPalette::Base));
}

VOID_NAMESPACE_CLOSE
