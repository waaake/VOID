/* STD */
#include <string>
#include <thread>

/* Qt */
#include <QLayout>
#include <QMenuBar>
#include <QIcon>
#include <QStyle>
#include <QValidator>

/* Internal */
#include "Browser.h"
#include "PlayerWindow.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

VoidMainWindow::VoidMainWindow(QWidget* parent)
    : QMainWindow(parent)
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
    m_Player->deleteLater();
}

QSize VoidMainWindow::sizeHint() const
{
    return QSize(1280, 720);
}

void VoidMainWindow::showEvent(QShowEvent* event)
{
    /* Set Default dock size */
    resizeDocks(m_DockList, m_DockSizes, Qt::Horizontal);
}

void VoidMainWindow::Build()
{
    /* Base */
    QWidget* baseWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(baseWidget);

    /* Player */
    m_Player = new Player();

    /* Add to the base layout */
    // layout->addWidget(m_Player);

    /* Media Lister Widget */
    m_MediaLister = new VoidMediaLister;

    /* Docker */
    m_Docker = new VoidDocker("Viewer", this);
    m_Docker->SetClosable(false);
    //m_Docker->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_MListDocker = new VoidDocker("Media", this);
    //m_MListDocker->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    /* Set the central widget */
    //setCentralWidget(baseWidget);
    //setCentralWidget(m_Docker);
    addDockWidget(Qt::RightDockWidgetArea, m_Docker);
    addDockWidget(Qt::LeftDockWidgetArea, m_MListDocker);

    /* Add to docker */
    m_Docker->setWidget(m_Player);
    m_MListDocker->setWidget(m_MediaLister);

    /* The way how dock widgets appear as default */
    /* Dock Widgets */
    m_DockList << m_Docker << m_MListDocker;
    /* Default Size Corresponding to each of the dock widget */
    m_DockSizes << 980 << 300;
    // resizeDocks(m_DockList, m_DockSizes, Qt::Horizontal);
    // m_MListDocker->resize(300, m_MListDocker->height());

    /* Menubar */
    QMenuBar* menuBar = new QMenuBar;

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

    /* Add to the Menubar */
    menuBar->addMenu(m_FileMenu);
    menuBar->addMenu(m_PlaybackMenu);

    /* Set Menubar on the parent window */
    setMenuBar(menuBar);
}

void VoidMainWindow::Connect()
{
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

    /* Media Lister */
    connect(m_MediaLister, &VoidMediaLister::mediaChanged, this, &VoidMainWindow::SetMedia);
    connect(m_MediaLister, &VoidMediaLister::mediaDropped, this, &VoidMainWindow::ImportMedia);

    /* Sequence */
    connect(m_Sequence.get(), &PlaybackSequence::rangeChanged, m_Player, &Player::SetRange);
}

void VoidMainWindow::CacheLookAhead()
{
    /*
     * Check if we're supposed to cache look ahead for the media
     * and if we're not already caching the media
     */
    if (m_CacheMedia & !m_Media.Caching())
    {
        /*
         * Get the current media to cache frames on a thread
         * Any frame which gets clicked on, in the timeslider caches the frame if not cached
         * meanwhile this cache continues on
         */
        std::thread t(&Media::Cache, m_Media);
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

    /* Add Media to the track after Clearing the existing contents */
    m_Track->Clear();

    // /* Set the start frame from the media in this case */
    // m_Track->SetStartFrame(m_Media.FirstFrame());
    m_Track->AddMedia(m_Media);

    /* Set the sequence on the Player */
    m_Player->Load(m_Sequence);

    /* Add the media on the Media Lister */
    m_MediaLister->AddMedia(m_Media);
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

void VoidMainWindow::SetMedia(const Media& media)
{
    /* Clear the player */
    m_Player->Clear();

    /* Update the media */
    m_Media = media;

    /* Set the sequence on the Player */
    m_Player->Load(m_Media);
}

void VoidMainWindow::AddMedia(const Media& media)
{
    /* Clear the player */
    m_Player->Clear();

    /* Add Media to the track */
    m_Track->AddMedia(media);

    /* Set the sequence on the Player */
    m_Player->Load(m_Sequence);
}

VOID_NAMESPACE_CLOSE
