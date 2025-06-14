/* STD */
#include <string>
#include <thread>

/* Qt */
#include <QLayout>
#include <QIcon>
#include <QPainter>
#include <QStyle>
#include <QValidator>

/* Internal */
#include "Browser.h"
#include "PlayerWindow.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* Docker Window {{{ */

DockerWindow::DockerWindow(QWidget* parent)
    : QMainWindow(parent)
{
    /* Build the Docker */
    Build();

    /* Set Docking options */
    setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks);
}

DockerWindow::~DockerWindow()
{
    m_Player->deleteLater();
}

void DockerWindow::Build()
{
    /* Player */
    m_Player = new Player();

    /* Media Lister Widget */
    m_MediaLister = new VoidMediaLister(this);

    /* Docker */
    m_Docker = new VoidDocker("Viewer", this);
    m_Docker->SetClosable(false);

    m_MListDocker = new VoidDocker("Media", this);

    /* Update widgets for the docks */
    m_Docker->setWidget(m_Player);
    m_MListDocker->setWidget(m_MediaLister);

    /* Set the central widget */
    addDockWidget(Qt::RightDockWidgetArea, m_Docker);
    addDockWidget(Qt::LeftDockWidgetArea, m_MListDocker);

    /* The way how dock widgets appear as default */
    /* Dock Widgets */
    m_DockList << m_Docker << m_MListDocker;
    /* Default Size Corresponding to each of the dock widget */
    m_DockSizes << 980 << 300;

    /* Resize Default docks */
    resizeDocks(m_DockList, m_DockSizes, Qt::Horizontal);
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
    switch (component)
    {
        case Component::MediaLister:
            /* Setup the Dock */
            ToggleDock(m_MListDocker, state, Qt::LeftDockWidgetArea);
            break;
    }
}

/* }}} */

VoidMainWindow::VoidMainWindow(QWidget* parent)
    : BaseWindow(parent)
    , m_CacheMedia(false)
    , m_Media()
{
    /* The default playback sequence */
    m_Sequence = std::make_shared<PlaybackSequence>();
    m_Track = std::make_shared<PlaybackTrack>(m_Sequence.get());

    /* Update the sequence with this track */
    m_Sequence->AddVideoTrack(m_Track);

    /* Build the UI */
    Build();

    /* Set the window title */
    setWindowTitle("VOID");

    /* Connect Signals -> Slots */
    Connect();
}

VoidMainWindow::~VoidMainWindow()
{
}

QSize VoidMainWindow::sizeHint() const
{
    return QSize(1280, 760);
}

void VoidMainWindow::showEvent(QShowEvent* event)
{
    /* Set Default dock size */
    // resizeDocks(m_DockList, m_DockSizes, Qt::Horizontal);
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

    /* Fetch the Player and Media Lister from the internal Docker */
    m_Player = m_InternalDocker->GetPlayer();
    m_MediaLister = m_InternalDocker->MediaLister();

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

    /* File Menu {{{ */
    m_FileMenu = new QMenu("File", menuBar);

    m_OpenAction = new QAction("Open...", m_FileMenu);
    m_OpenAction->setShortcut(QKeySequence("Ctrl+O"));

    m_ClearAction = new QAction("Clear", m_FileMenu);

    m_CloseAction = new QAction("Close Player", m_FileMenu);
    m_CloseAction->setShortcut(QKeySequence("Ctrl+Q"));

    m_FileMenu->addAction(m_OpenAction);
    m_FileMenu->addAction(m_ClearAction);
    m_FileMenu->addAction(m_CloseAction);
    /* }}} */

    /* Playback Menu {{{ */
    m_PlaybackMenu = new QMenu("Playback", menuBar);

    m_EnableCacheAction = new QAction("Enable Look Ahead Caching", m_PlaybackMenu);
    m_DisableCacheAction = new QAction("Disable Look Ahead Caching", m_PlaybackMenu);
    m_StopCacheAction = new QAction("Stop Caching Media", m_PlaybackMenu);
    m_ResumeCacheAction = new QAction("Resume Caching Media", m_PlaybackMenu);
    m_ClearCacheAction = new QAction("Clear Cache", m_PlaybackMenu);

    m_PlayForwardsAction = new QAction("Play Forwards", m_PlaybackMenu);
    m_PlayForwardsAction->setShortcut(QKeySequence(Qt::Key_L));

    m_StopPlayingAction = new QAction("Stop Playing", m_PlaybackMenu);
    m_StopPlayingAction->setShortcut(QKeySequence(Qt::Key_K));

    m_PlayBackwardsAction = new QAction("Play Backwards", m_PlaybackMenu);
    m_PlayBackwardsAction->setShortcut(QKeySequence(Qt::Key_J));

    m_ForwardsAction = new QAction("Go to Next Frame", m_PlaybackMenu);
    m_ForwardsAction->setShortcut(QKeySequence(Qt::Key_Period));

    m_BackwardsAction = new QAction("Go to Previous Frame", m_PlaybackMenu);
    m_BackwardsAction->setShortcut(QKeySequence(Qt::Key_Comma));

    m_EndFrameAction = new QAction("Go to End", m_PlaybackMenu);
    m_EndFrameAction->setShortcut(QKeySequence(Qt::Key_PageDown));

    m_StartFrameAction = new QAction("Go to Start", m_PlaybackMenu);
    m_StartFrameAction->setShortcut(QKeySequence(Qt::Key_PageUp));

    /* Playback Cache Actions */
    m_PlaybackMenu->addAction(m_EnableCacheAction);
    m_PlaybackMenu->addAction(m_DisableCacheAction);
    m_PlaybackMenu->addAction(m_StopCacheAction);
    m_PlaybackMenu->addAction(m_ResumeCacheAction);
    m_PlaybackMenu->addAction(m_ClearCacheAction);

    /* -------------------------------- */
    m_PlaybackMenu->insertSeparator(m_PlayForwardsAction);

    /* Playback Actions */
    m_PlaybackMenu->addAction(m_PlayForwardsAction);
    m_PlaybackMenu->addAction(m_PlayBackwardsAction);
    m_PlaybackMenu->addAction(m_StopPlayingAction);
    m_PlaybackMenu->addAction(m_ForwardsAction);
    m_PlaybackMenu->addAction(m_BackwardsAction);
    m_PlaybackMenu->addAction(m_StartFrameAction);
    m_PlaybackMenu->addAction(m_EndFrameAction);
    /* }}} */

    /* Viewer Contols Menu {{{ */
    m_ViewerMenu = new QMenu("Viewer", menuBar);

    m_ZoomInAction = new QAction("Zoom In");
    m_ZoomInAction->setShortcut(QKeySequence(Qt::Key_Plus));

    m_ZoomOutAction = new QAction("Zoom Out");
    m_ZoomOutAction->setShortcut(QKeySequence(Qt::Key_Minus));

    m_ZoomToFitAction = new QAction("Zoom to Fit");
    m_ZoomToFitAction->setShortcut(QKeySequence(Qt::Key_F));

    m_ViewerMenu->addAction(m_ZoomInAction);
    m_ViewerMenu->addAction(m_ZoomOutAction);
    m_ViewerMenu->addAction(m_ZoomToFitAction);
    /* }}} */

    /* Window Menu {{{ */
    m_WindowMenu = new QMenu("Window", menuBar);

    /* All Window/Component Menu Actions are checkable */
    m_MediaListerAction = new QAction("Media List", m_WindowMenu);
    m_MediaListerAction->setCheckable(true);

    m_WindowMenu->addAction(m_MediaListerAction);
    /* }}} */

    /* Help Menu {{{ */
    m_HelpMenu = new QMenu("Help", menuBar);

    m_AboutAction = new QAction("About VOID");

    m_HelpMenu->addAction(m_AboutAction);
    /* }}} */

    /* Add to the Menubar */
    menuBar->addMenu(m_FileMenu);
    menuBar->addMenu(m_PlaybackMenu);
    menuBar->addMenu(m_ViewerMenu);
    menuBar->addMenu(m_WindowMenu);
    menuBar->addMenu(m_HelpMenu);
}

void VoidMainWindow::Connect()
{
    #ifndef USE_FRAMED_WINDOW   /* Not using Framed window */
    /* Title Bar Actions */
    connect(m_TitleBar, &VoidTitleBar::requestMinimize, this, &QWidget::showMinimized);
    connect(m_TitleBar, &VoidTitleBar::requestMaximizeRestore, this, [this]() { isMaximized() ? showNormal() : showMaximized(); });
    connect(m_TitleBar, &VoidTitleBar::requestClose, this, &QWidget::close);
    #endif  // USE_FRAMED_WINDOW

    /* Menu Actions */
    /* File Menu {{{ */
    connect(m_CloseAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_OpenAction, SIGNAL(triggered()), this, SLOT(Load()));
    connect(m_ClearAction, SIGNAL(triggered()), m_Player, SLOT(Clear()));
    /* }}} */

    /* Playback Menu {{{ */
    connect(m_EnableCacheAction, &QAction::triggered, this, [this](){ ToggleLookAheadCache(true); });
    connect(m_DisableCacheAction, &QAction::triggered, this, [this](){ ToggleLookAheadCache(false); });
    connect(m_StopCacheAction, &QAction::triggered, this, [this](){ m_Media.StopCaching(); });
    connect(m_ClearCacheAction, &QAction::triggered, this, &VoidMainWindow::ClearLookAheadCache);

    connect(m_PlayForwardsAction, &QAction::triggered, m_Player, &Player::PlayForwards);
    connect(m_PlayBackwardsAction, &QAction::triggered, m_Player, &Player::PlayBackwards);
    connect(m_StopPlayingAction, &QAction::triggered, m_Player, &Player::Stop);
    connect(m_ForwardsAction, &QAction::triggered, m_Player, &Player::NextFrame);
    connect(m_BackwardsAction, &QAction::triggered, m_Player, &Player::PreviousFrame);
    connect(m_StartFrameAction, &QAction::triggered, m_Player, &Player::MoveToStart);
    connect(m_EndFrameAction, &QAction::triggered, m_Player, &Player::MoveToEnd);
    /* }}} */

    /* Viewer Menu {{{ */
    connect(m_ZoomInAction, &QAction::triggered, m_Player, &Player::ZoomIn);
    connect(m_ZoomOutAction, &QAction::triggered, m_Player, &Player::ZoomOut);
    connect(m_ZoomToFitAction, &QAction::triggered, m_Player, &Player::ZoomToFit);
    /* }}} */

    connect(m_MediaListerAction, &QAction::toggled, this, [this](bool checked) { m_InternalDocker->ToggleComponent(DockerWindow::Component::MediaLister, checked); });
    /* }}} */

    /* Help Menu {{{ */
    connect(m_AboutAction, &QAction::triggered, this, [this]() { AboutVoid(this).exec(); });
    /* }}} */

    /* Media Lister */
    connect(m_MediaLister, &VoidMediaLister::mediaChanged, this, &VoidMainWindow::SetMedia);
    connect(m_MediaLister, &VoidMediaLister::mediaDropped, this, &VoidMainWindow::ImportMedia);
    connect(m_MediaLister, &VoidMediaLister::playlistChanged, this, &VoidMainWindow::PlayMedia);

    /* Sequence */
    connect(m_Sequence.get(), &PlaybackSequence::rangeChanged, m_Player, &Player::SetRange);

    /* Track */
    connect(m_Track.get(), &PlaybackTrack::frameCached, m_Player, &Player::AddCacheFrame);
    connect(m_Track.get(), &PlaybackTrack::cacheCleared, m_Player, &Player::ClearCachedFrames);
}

void VoidMainWindow::CacheLookAhead()
{
    /*
     * Check if we're supposed to cache look ahead for the media
     * and if we're not already caching the media
     */
    if (m_CacheMedia)
    {
        std::thread t;

        /* Grab the Active Media Clip, if that's set on the Player */
        SharedMediaClip clip = m_Player->ActiveMediaClip();

        /*
         * Get the current media to cache frames on a thread
         * Any frame which gets clicked on, in the timeslider caches the frame if not cached
         * meanwhile this cache continues on
         */
        if (clip)
            t = std::thread(&MediaClip::Cache, clip.get());
        else
            t = std::thread(&PlaybackTrack::Cache, m_Track.get());

        /* TODO: Replace with threadpool */
        /*
        * This shouldn't be all bad here but we can still have issues if the media is caching and
        * it was changed, at the moment, we can always call stop caching before chaning media
        */
        /* Detach so that this continues on irrespective of the scope of the function */
        t.detach();
    }
}

void VoidMainWindow::ClearLookAheadCache()
{
    /* Clear any data from the memory which was cached to improve playback */
    if (m_Media.Valid())
    {
        m_Media.ClearCache();
    }

    /* Clear Cache from the track */
    m_Track->ClearCache();
}

void VoidMainWindow::ToggleLookAheadCache(const bool toggle)
{
    /* Update the State for Caching media */
    m_CacheMedia = toggle;

    /*
     * Since there is no explicit user-option provided to begin cache
     * Calling CacheLookAhead here to begin the cache if CacheMedia was set to 1
     * Setting CacheMedia to 0, should not clear any existing cache so this case should suffice it
     */
    CacheLookAhead();
}

void VoidMainWindow::ImportMedia(const std::string& path)
{
    /* Update the media from the path */
    m_Media.Read(path);

    /* Cache */
    CacheLookAhead();

    // /* Set the start frame from the media in this case */
    // m_Track->SetStartFrame(m_Media.FirstFrame());
    SharedMediaClip clip = std::make_shared<MediaClip>(m_Media);

    /* Load the clip on the player */
    m_Player->Load(clip);

    /* Add the media clip on the Media Lister */
    m_MediaLister->AddMedia(clip);
}

// Slots
void VoidMainWindow::Load()
{
    VoidMediaBrowser mediaBrowser;

    /* In case the dialog was not accepted */
    if (!mediaBrowser.Browse())
    {
        VOID_LOG_INFO("User Cancelled Browsing.");
        return;
    }

    /* Read the directory from the FileDialog */
    ImportMedia(mediaBrowser.GetDirectory());
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

    for (SharedMediaClip m: media)
    {
        /* Add Media to the track */
        m_Track->AddMedia(m);
    }

    /* Set the sequence on the Player */
    m_Player->Load(m_Sequence);
}

VOID_NAMESPACE_CLOSE
