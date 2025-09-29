// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QCoreApplication>
#include <QLayout>
#include <QIcon>
#include <QPainter>

/* Internal */
#include "PlayerWindow.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Dock/DockManager.h"
#include "VoidUi/Engine/IconForge.h"
#include "VoidUi/Project/ProjectBridge.h"
#include "VoidUi/Preferences/PreferencesUI.h"

VOID_NAMESPACE_OPEN

/* Docker Window {{{ */

DockerWindow::DockerWindow(QWidget* parent)
    : QMainWindow(parent)
{
    /* Build the Docker */
    Build();
}

DockerWindow::~DockerWindow()
{
}

void DockerWindow::Build()
{
    m_DockSizes << 400 << 700;

    /* Docker */
    m_Splitter = new DockSplitter(Qt::Horizontal, this);

    /* Add Panels */
    m_Splitter->AddPane(static_cast<int>(Component::MediaLister));
    m_Splitter->AddPane(static_cast<int>(Component::Viewer));

    m_Splitter->setSizes(m_DockSizes);

    setCentralWidget(m_Splitter);
}

void DockerWindow::ToggleDock(VoidDocker* dock, const bool state, const Qt::DockWidgetArea& area)
{
    /* Set the visibility of the component */
    dock->setVisible(state);

    /* And redock to said area */
    if (state)
    {
        addDockWidget(area, dock);
    }
}

void DockerWindow::ToggleComponent(const Component& component, const bool state)
{
    // switch (component)
    // {
    //     case Component::MediaLister:
    //         /* Setup the Dock */
    //         ToggleDock(m_MListDocker, state, Qt::LeftDockWidgetArea);
    //         break;
    // }
}

/* }}} */

VoidMainWindow::VoidMainWindow(QWidget* parent)
    : BaseWindow(parent)
    , m_Bridge(MBridge::Instance())
{
    /* The default playback sequence */
    m_Sequence = std::make_shared<PlaybackSequence>();
    m_Track = std::make_shared<PlaybackTrack>(m_Sequence.get());

    /* Update the sequence with this track */
    m_Sequence->AddVideoTrack(m_Track);

    RegisterDocks();
    Build();

    setWindowTitle("VOID");
    setAttribute(Qt::WA_QuitOnClose);

    /* Connect Signals -> Slots */
    Connect();
}

VoidMainWindow::~VoidMainWindow()
{
    m_Player->deleteLater();
    m_MediaLister->deleteLater();
}

void VoidMainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().color(QPalette::Dark));
}

QSize VoidMainWindow::sizeHint() const
{
    return QSize(1280, 760);
}

QMenuBar* VoidMainWindow::MenuBar() const
{
    #ifdef USE_FRAMED_WINDOW
    return menuBar();
    #else
    /* Menubar from the TitleBar */
    return m_TitleBar->MenuBar();
    #endif // USE_FRAMED_WINDOW
}

void VoidMainWindow::Build()
{
    /* Base Frameless widget */
    QWidget* baseWidget = new QWidget;
    /* Base Layout to which first layer gets added*/
    QVBoxLayout* layout = new QVBoxLayout(baseWidget);

    /* Don't want any extra margins */
    layout->setContentsMargins(2, 0, 2, 2);

    #ifndef USE_FRAMED_WINDOW       /* Not Using Framed window */
    /*
     * The Main title bar for the Void Window
     * This will act as a drop-in replacement for the standard TitleBar OS specific
     */
    m_TitleBar = new VoidTitleBar(this);
    layout->addWidget(m_TitleBar, 0, Qt::AlignTop);
    #endif // USE_FRAMED_WINDOW

    /*
     * This is the internal window holding all of the components inside
     * and is a docker window where components could be docked/undocked and moved
     * All of the internal components like player, media list, timeline etc. would exist here
     */
    m_InternalDocker = new DockerWindow(this);
    layout->addWidget(m_InternalDocker);

    /* Set the central widget */
    setCentralWidget(baseWidget);

    /**
     * If we're using Framed window which means the menu bar now has to be part of the
     * Window as a menu bar
     * Else we use the custom defined menubar from the TitleBar
     */
    #ifdef USE_FRAMED_WINDOW
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    #else
    /* Menubar from the TitleBar */
    QMenuBar* menuBar = m_TitleBar->MenuBar();
    #endif // USE_FRAMED_WINDOW
}

void VoidMainWindow::Connect()
{
    #ifndef USE_FRAMED_WINDOW   /* Not using Framed window */
    /* Title Bar Actions */
    connect(m_TitleBar, &VoidTitleBar::requestMinimize, this, &QWidget::showMinimized);
    connect(m_TitleBar, &VoidTitleBar::requestMaximizeRestore, this, [this]() { isMaximized() ? showNormal() : showMaximized(); });
    connect(m_TitleBar, &VoidTitleBar::requestClose, this, &QCoreApplication::quit);
    #endif  // USE_FRAMED_WINDOW

    /* Media Lister */
    connect(m_MediaLister, &VoidMediaLister::mediaChanged, this, &VoidMainWindow::SetMedia);
    connect(m_MediaLister, &VoidMediaLister::playlistChanged, this, &VoidMainWindow::PlayMedia);
    connect(m_MediaLister, &VoidMediaLister::metadataInspected, this, &VoidMainWindow::InspectMetadata);

    /* Play Lister */
    connect(m_PlayLister, &VoidPlayLister::playlistChanged, this, &VoidMainWindow::PlayMedia);

    /* Sequence */
    connect(m_Sequence.get(), &PlaybackSequence::rangeChanged, m_Player, &Player::SetRange);

    /* Track */
    connect(m_Track.get(), &PlaybackTrack::frameCached, m_Player, &Player::AddCacheFrame);
    connect(m_Track.get(), &PlaybackTrack::cacheCleared, m_Player, &Player::ClearCachedFrames);
}

void VoidMainWindow::RegisterDocks()
{
    /* Register Dock Widgets in the Dock Manager */
    DockManager& manager = DockManager::Instance();

    /* Player */
    m_Player = new Player(this);

    /* Media Lister Widget */
    m_MediaLister = new VoidMediaLister(this);
    m_PlayLister = new VoidPlayLister();

    /* Python Script Editor */
    m_ScriptEditor = new PyScriptEditor();

    /* Media Metadata Viewer */
    m_MetadataViewer = new MetadataViewer();

    manager.RegisterDock(m_MediaLister, "Media View");
    manager.RegisterDock(m_Player, "Viewer");
    manager.RegisterDock(m_ScriptEditor, "Script Editor");
    manager.RegisterDock(m_MetadataViewer, "Metadata Viewer");
    manager.RegisterDock(m_PlayLister, "Playlist View");
}

void VoidMainWindow::InitMenu(MenuSystem* menuSystem)
{
    /* File Menu {{{ */
    QMenu* fileMenu = menuSystem->AddMenu("File");

    QAction* importAction = new QAction("Import Media...", fileMenu);
    importAction->setShortcut(QKeySequence("Ctrl+I"));

    QAction* importDirectoryAction = new QAction("Import Directory...", fileMenu);
    importDirectoryAction->setShortcut(QKeySequence("Ctrl+Alt+I"));
    
    QAction* newProjectAction = new QAction("New Project", fileMenu);
    newProjectAction->setShortcut(QKeySequence("Ctrl+N"));

    QAction* saveProjectAction = new QAction("Save Project", fileMenu);
    saveProjectAction->setShortcut(QKeySequence("Ctrl+S"));

    QAction* saveAsProjectAction = new QAction("Save Project As...", fileMenu);
    saveAsProjectAction->setShortcut(QKeySequence("Ctrl+Shift+S"));

    QAction* loadProjectAction = new QAction("Open Project...", fileMenu);
    loadProjectAction->setShortcut(QKeySequence("Ctrl+O"));

    QAction* closeProjectAction = new QAction("Close Project", fileMenu);
    closeProjectAction->setShortcut(QKeySequence("Ctrl+W"));

    QAction* clearAction = new QAction("Clear", fileMenu);

    QAction* closeAction = new QAction("Close Player", fileMenu);
    closeAction->setShortcut(QKeySequence("Ctrl+Q"));

    fileMenu->addAction(importAction);
    fileMenu->addAction(importDirectoryAction);

    /* -------------------------------- */
    fileMenu->addSeparator();

    fileMenu->addAction(newProjectAction);
    fileMenu->addAction(saveProjectAction);
    fileMenu->addAction(saveAsProjectAction);
    fileMenu->addAction(loadProjectAction);
    fileMenu->addAction(closeProjectAction);

    /* -------------------------------- */
    fileMenu->addSeparator();

    fileMenu->addAction(clearAction);
    fileMenu->addAction(closeAction);

    connect(closeAction, &QAction::triggered, this, &QCoreApplication::quit);
    connect(importAction, &QAction::triggered, this, []() -> void { _ProjectBridge.ImportMedia(); });
    connect(importDirectoryAction, &QAction::triggered, this, []() -> void { _ProjectBridge.ImportDirectory(); });
    connect(newProjectAction, &QAction::triggered, this, [this]() { _MediaBridge.NewProject(); });
    connect(saveProjectAction, &QAction::triggered, this, []() -> void { _ProjectBridge.Save(); });
    connect(saveAsProjectAction, &QAction::triggered, this, []() -> void { _ProjectBridge.SaveAs(); });
    connect(loadProjectAction, &QAction::triggered, this, []() -> void { _ProjectBridge.Open(); });
    connect(closeProjectAction, &QAction::triggered, this, []() -> void { _ProjectBridge.Close(); });
    connect(clearAction, &QAction::triggered, m_Player, &Player::Clear);

    /* }}} */

    /* Edit Menu {{{ */
    QMenu* editMenu = menuSystem->AddMenu("Edit");

    QAction* undoAction = m_Bridge.CreateUndoAction(editMenu);
    undoAction->setShortcut(QKeySequence("Ctrl+Z"));
    
    QAction* redoAction = m_Bridge.CreateRedoAction(editMenu);
    redoAction->setShortcut(QKeySequence("Ctrl+Shift+Z"));

    QAction* editPrefsAction = new QAction("Preferences...", editMenu);
    QIcon icon;
    icon.addPixmap(IconForge::GetPixmap(IconType::icon_build, _DARK_COLOR(QPalette::Text, 150), 18), QIcon::Normal);
    icon.addPixmap(IconForge::GetPixmap(IconType::icon_build, _COLOR(QPalette::Dark), 18), QIcon::Active);
    editPrefsAction->setIcon(icon);
    
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);

    /* -------------------------------- */
    editMenu->addSeparator();

    editMenu->addAction(editPrefsAction);

    connect(editPrefsAction, &QAction::triggered, this, [this]() { VoidPreferencesWidget(this).exec(); } );
    /* }}} */

    /* Playback Menu {{{ */
    QMenu* playbackMenu = menuSystem->AddMenu("Playback");

    QAction* enableCacheAction = new QAction("Enable Look Ahead Caching", playbackMenu);
    QAction* disableCacheAction = new QAction("Disable Look Ahead Caching", playbackMenu);
    QAction* stopCacheAction = new QAction("Stop Caching Media", playbackMenu);
    QAction* refreshCacheAction = new QAction("Refresh Media Cache", playbackMenu);
    QAction* resumeCacheAction = new QAction("Resume Caching Media", playbackMenu);
    QAction* clearCacheAction = new QAction("Clear Cache", playbackMenu);

    QAction* playForwardsAction = new QAction("Play Forwards", playbackMenu);
    playForwardsAction->setShortcut(QKeySequence(Qt::Key_L));

    QAction* stopPlayingAction = new QAction("Stop Playing", playbackMenu);
    stopPlayingAction->setShortcut(QKeySequence(Qt::Key_K));

    QAction* playBackwardsAction = new QAction("Play Backwards", playbackMenu);
    playBackwardsAction->setShortcut(QKeySequence(Qt::Key_J));

    QAction* forwardsAction = new QAction("Go to Next Frame", playbackMenu);
    forwardsAction->setShortcut(QKeySequence(Qt::Key_Period));

    QAction* backwardsAction = new QAction("Go to Previous Frame", playbackMenu);
    backwardsAction->setShortcut(QKeySequence(Qt::Key_Comma));

    QAction* endFrameAction = new QAction("Go to End", playbackMenu);
    endFrameAction->setShortcut(QKeySequence(Qt::Key_PageDown));

    QAction* startFrameAction = new QAction("Go to Start", playbackMenu);
    startFrameAction->setShortcut(QKeySequence(Qt::Key_PageUp));

    QAction* setInFrameAction = new QAction("Set In Frame", playbackMenu);
    setInFrameAction->setShortcut(QKeySequence(Qt::Key_BracketLeft));

    QAction* setOutFrameAction = new QAction("Set Out Frame", playbackMenu);
    setOutFrameAction->setShortcut(QKeySequence(Qt::Key_BracketRight));

    QAction* resetRangeAction = new QAction("Reset In/Out Frames", playbackMenu);
    resetRangeAction->setShortcut(QKeySequence(Qt::Key_Backslash));

    /* Playback Cache Actions */
    playbackMenu->addAction(enableCacheAction);
    playbackMenu->addAction(disableCacheAction);
    playbackMenu->addAction(stopCacheAction);
    playbackMenu->addAction(refreshCacheAction);
    playbackMenu->addAction(resumeCacheAction);
    playbackMenu->addAction(clearCacheAction);

    /* -------------------------------- */
    playbackMenu->insertSeparator(playForwardsAction);

    /* Playback Actions */
    playbackMenu->addAction(playForwardsAction);
    playbackMenu->addAction(playBackwardsAction);
    playbackMenu->addAction(stopPlayingAction);
    playbackMenu->addAction(forwardsAction);
    playbackMenu->addAction(backwardsAction);
    playbackMenu->addAction(startFrameAction);
    playbackMenu->addAction(endFrameAction);

    /* -------------------------------- */
    playbackMenu->insertSeparator(setInFrameAction);

    playbackMenu->addAction(setInFrameAction);
    playbackMenu->addAction(setOutFrameAction);
    playbackMenu->addAction(resetRangeAction);

    connect(enableCacheAction, &QAction::triggered, m_Player, &Player::ResumeCache);
    connect(disableCacheAction, &QAction::triggered, m_Player, &Player::DisableCache);
    connect(stopCacheAction, &QAction::triggered, m_Player, &Player::StopCache);
    connect(refreshCacheAction, &QAction::triggered, m_Player, &Player::Recache);
    connect(resumeCacheAction, &QAction::triggered, m_Player, &Player::ResumeCache);
    connect(clearCacheAction, &QAction::triggered, m_Player, &Player::ClearCache);

    connect(playForwardsAction, &QAction::triggered, m_Player, &Player::PlayForwards);
    connect(playBackwardsAction, &QAction::triggered, m_Player, &Player::PlayBackwards);
    connect(stopPlayingAction, &QAction::triggered, m_Player, &Player::Stop);
    connect(forwardsAction, &QAction::triggered, m_Player, &Player::NextFrame);
    connect(backwardsAction, &QAction::triggered, m_Player, &Player::PreviousFrame);
    connect(startFrameAction, &QAction::triggered, m_Player, &Player::MoveToStart);
    connect(endFrameAction, &QAction::triggered, m_Player, &Player::MoveToEnd);
    connect(setInFrameAction, &QAction::triggered, m_Player, &Player::ResetInFrame);
    connect(setOutFrameAction, &QAction::triggered, m_Player, &Player::ResetOutFrame);
    connect(resetRangeAction, &QAction::triggered, m_Player, &Player::ResetRange);
    /* }}} */

    /* Viewer Contols Menu {{{ */
    QMenu* viewerMenu = menuSystem->AddMenu("Viewer");

    QAction* zoomInAction = new QAction("Zoom In", viewerMenu);
    zoomInAction->setShortcut(QKeySequence(Qt::Key_Plus));

    QAction* zoomOutAction = new QAction("Zoom Out", viewerMenu);
    zoomOutAction->setShortcut(QKeySequence(Qt::Key_Minus));

    QAction* zoomToFitAction = new QAction("Zoom to Fit", viewerMenu);
    zoomToFitAction->setShortcut(QKeySequence(Qt::Key_F));

    QAction* fullscreenAction = new QAction("Show Fullscreen", viewerMenu);
    fullscreenAction->setShortcut(QKeySequence("Ctrl+F"));
    
    QAction* exitFullscreenAction = new QAction("Exit Fullscreen", viewerMenu);

    viewerMenu->addAction(zoomInAction);
    viewerMenu->addAction(zoomOutAction);
    viewerMenu->addAction(zoomToFitAction);
    viewerMenu->addAction(fullscreenAction);
    viewerMenu->addAction(exitFullscreenAction);

    connect(zoomInAction, &QAction::triggered, m_Player, &Player::ZoomIn);
    connect(zoomOutAction, &QAction::triggered, m_Player, &Player::ZoomOut);
    connect(zoomToFitAction, &QAction::triggered, m_Player, &Player::ZoomToFit);
    connect(fullscreenAction, &QAction::triggered, m_Player, &Player::SetRendererFullscreen);
    connect(exitFullscreenAction, &QAction::triggered, m_Player, &Player::ExitFullscreenRenderer);
    /* }}} */

    /* Help Menu {{{ */
    QMenu* helpMenu = menuSystem->AddMenu("Help");

    QAction* aboutAction = new QAction("About VOID", helpMenu);
    helpMenu->addAction(aboutAction);

    connect(aboutAction, &QAction::triggered, this, [this]() { AboutVoid(this).exec(); });
    /* }}} */
}

void VoidMainWindow::SetMedia(const SharedMediaClip& media)
{
    /* Clear the player */
    m_Player->Clear();

    /* Set the Clip on the player */
    m_Player->Load(media);
}

void VoidMainWindow::AddMedia(const SharedMediaClip& media)
{
    /* Clear the player */
    m_Player->Clear();

    /* Add Media to the track */
    m_Track->AddMedia(media);

    /* Set the sequence on the Player */
    m_Player->Load(m_Sequence);
}

void VoidMainWindow::PlayMedia(const std::vector<SharedMediaClip>& media)
{
    /* Clear the player */
    m_Player->Clear();

    /* Clear the track */
    m_Track->Clear();

    for (SharedMediaClip m : media)
        m_Track->AddMedia(m);

    /* Set the sequence on the Player */
    m_Player->Load(m_Sequence);
}

void VoidMainWindow::InspectMetadata(const SharedMediaClip& media)
{
    m_MetadataViewer->SetFromMedia(media);
}

VOID_NAMESPACE_CLOSE
