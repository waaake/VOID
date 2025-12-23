// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "PlayerBridge.h"

VOID_NAMESPACE_OPEN

PlayerBridge::PlayerBridge()
{
    m_Player = new Player();
    m_Playlist = new Playlist();
}

PlayerBridge::~PlayerBridge()
{
    /**
     * This already gets deleted when the main window is destroyed
     * as this belongs to an internal layout from that, so that internally
     * marks this for deletion when the layout is destroyed
     */
    // m_Player->deleteLater();
    // delete m_Player;
    // m_Player = nullptr;

    m_Playlist->deleteLater();
    delete m_Playlist;
    m_Playlist = nullptr;
}

PlayerBridge& PlayerBridge::Instance()
{
    static PlayerBridge instance;
    return instance;
}

void PlayerBridge::InitMenu(MenuSystem* menuSystem)
{
    /* Playback Menu {{{ */
    QMenu* playbackMenu = menuSystem->AddMenu("Playback");

    QAction* enableCacheAction = menuSystem->AddAction(playbackMenu, "Enable Look Ahead Caching");
    QAction* disableCacheAction = menuSystem->AddAction(playbackMenu, "Disable Look Ahead Caching");
    QAction* stopCacheAction = menuSystem->AddAction(playbackMenu, "Stop Caching Media");
    QAction* refreshCacheAction = menuSystem->AddAction(playbackMenu, "Refresh Media Cache");
    QAction* resumeCacheAction = menuSystem->AddAction(playbackMenu, "Resume Caching Media");
    QAction* clearCacheAction = menuSystem->AddAction(playbackMenu, "Clear Cache");

    /* -------------------------------- */
    playbackMenu->addSeparator();

    QAction* playForwardsAction = menuSystem->AddAction(playbackMenu, "Play Forwards", QKeySequence(Qt::Key_L));
    QAction* stopPlayingAction = menuSystem->AddAction(playbackMenu, "Stop Playing", QKeySequence(Qt::Key_K));
    QAction* playBackwardsAction = menuSystem->AddAction(playbackMenu, "Play Backwards", QKeySequence(Qt::Key_J));
    QAction* forwardsAction = menuSystem->AddAction(playbackMenu, "Go to Next Frame", QKeySequence(Qt::Key_Period));
    QAction* backwardsAction = menuSystem->AddAction(playbackMenu, "Go to Previous Frame", QKeySequence(Qt::Key_Comma));
    QAction* endFrameAction = menuSystem->AddAction(playbackMenu, "Go to End", QKeySequence(Qt::Key_PageDown));

    /* -------------------------------- */
    playbackMenu->addSeparator();

    QAction* startFrameAction = menuSystem->AddAction(playbackMenu, "Go to Start", QKeySequence(Qt::Key_PageUp));
    QAction* setInFrameAction = menuSystem->AddAction(playbackMenu, "Set In Frame", QKeySequence(Qt::Key_BracketLeft));
    QAction* setOutFrameAction = menuSystem->AddAction(playbackMenu, "Set Out Frame", QKeySequence(Qt::Key_BracketRight));
    QAction* resetRangeAction = menuSystem->AddAction(playbackMenu, "Reset In/Out Frames", QKeySequence(Qt::Key_Backslash));

    /* -------------------------------- */
    playbackMenu->addSeparator();

    QAction* inspectMetdataAction = menuSystem->AddAction(playbackMenu, "Inspect Metadata", QKeySequence(Qt::Key_I));

    connect(enableCacheAction, &QAction::triggered, this, &PlayerBridge::ResumeCache);
    connect(disableCacheAction, &QAction::triggered, this, &PlayerBridge::DisableCache);
    connect(stopCacheAction, &QAction::triggered, this, &PlayerBridge::StopCache);
    connect(refreshCacheAction, &QAction::triggered, this, &PlayerBridge::Recache);
    connect(resumeCacheAction, &QAction::triggered, this, &PlayerBridge::ResumeCache);
    connect(clearCacheAction, &QAction::triggered, this, &PlayerBridge::ClearCache);

    connect(playForwardsAction, &QAction::triggered, this, &PlayerBridge::PlayForwards);
    connect(playBackwardsAction, &QAction::triggered, this, &PlayerBridge::PlayBackwards);
    connect(stopPlayingAction, &QAction::triggered, this, &PlayerBridge::Stop);
    connect(forwardsAction, &QAction::triggered, this, &PlayerBridge::NextFrame);
    connect(backwardsAction, &QAction::triggered, this, &PlayerBridge::PreviousFrame);
    connect(startFrameAction, &QAction::triggered, this, &PlayerBridge::MoveToStart);
    connect(endFrameAction, &QAction::triggered, this, &PlayerBridge::MoveToEnd);
    connect(setInFrameAction, &QAction::triggered, this, &PlayerBridge::ResetInFrame);
    connect(setOutFrameAction, &QAction::triggered, this, &PlayerBridge::ResetOutFrame);
    connect(resetRangeAction, &QAction::triggered, this, &PlayerBridge::ResetRange);

    connect(inspectMetdataAction, &QAction::triggered, this, &PlayerBridge::InspectCurrentMetadata);
    /* }}} */

    /* Viewer Contols Menu {{{ */
    QMenu* viewerMenu = menuSystem->AddMenu("Viewer");

    QAction* zoomInAction = menuSystem->AddAction(viewerMenu, "Zoom In", QKeySequence(Qt::Key_Plus));
    QAction* zoomOutAction = menuSystem->AddAction(viewerMenu, "Zoom Out", QKeySequence(Qt::Key_Minus));
    QAction* zoomToFitAction = menuSystem->AddAction(viewerMenu, "Zoom to Fit", QKeySequence(Qt::Key_F));
    QAction* fullscreenAction = menuSystem->AddAction(viewerMenu, "Show Fullscreen", QKeySequence("Ctrl+F"));
    QAction* exitFullscreenAction = menuSystem->AddAction(viewerMenu, "Exit Fullscreen");

    connect(zoomInAction, &QAction::triggered, this, &PlayerBridge::ZoomIn);
    connect(zoomOutAction, &QAction::triggered, this, &PlayerBridge::ZoomOut);
    connect(zoomToFitAction, &QAction::triggered, this, &PlayerBridge::ZoomToFit);
    connect(fullscreenAction, &QAction::triggered, this, &PlayerBridge::SetFullscreen);
    connect(exitFullscreenAction, &QAction::triggered, this, &PlayerBridge::ExitFullscreen);
    /* }}} */
}

void PlayerBridge::AddToQueue(const SharedMediaClip& media, bool refresh)
{
    m_Playlist->AddMedia(media);

    if (refresh)
        m_Player->SetPlaylist(m_Playlist);
}

void PlayerBridge::AddToQueue(const std::vector<SharedMediaClip>& media, bool refresh)
{
    for (const SharedMediaClip& m : media)
        m_Playlist->AddMedia(m);

    if (refresh)
        m_Player->SetPlaylist(m_Playlist);
}

VOID_NAMESPACE_CLOSE
