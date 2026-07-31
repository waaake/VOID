// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "TimelineController.h"

VOID_NAMESPACE_OPEN

TimelineController::TimelineController(QObject* parent)
    : QObject(parent)
{
    m_Timeline = new Timeline();

    connect(m_Timeline, &Timeline::TimeChanged, this, &TimelineController::timeChanged, Qt::DirectConnection);
    connect(m_Timeline, &Timeline::fullscreenRequested, this, &TimelineController::fullscreenRequested, Qt::DirectConnection);
    connect(m_Timeline, &Timeline::playbackStateChanged, this, &TimelineController::playbackStateChanged, Qt::DirectConnection);
    connect(m_Timeline, &Timeline::mediaFinished, this, &TimelineController::mediaFinished, Qt::DirectConnection);
    connect(m_Timeline, &Timeline::seeked, this, &TimelineController::seeked);
}

TimelineController::~TimelineController()
{
    // Timeline was getting deleted earlier than the player, causing seg faults when closing the player
    // in case there's playback happening, this should get deleted automatically as it's a part of the player
    // when Qt calls deleteLater(), but in case this is flagged by any sanitizer for mem leaks, we could try and move
    // the deletion to a different place or check for states in the player
    // m_Timeline->deleteLater();
    // delete m_Timeline;
    // m_Timeline = nullptr;
}

VOID_NAMESPACE_CLOSE
