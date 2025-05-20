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

    m_FileMenu = new QMenu("File", menuBar);
    m_OpenAction = new QAction("Open...", m_FileMenu);
    m_OpenAction->setShortcut(QKeySequence("Ctrl+O"));
    m_ClearAction = new QAction("Clear", m_FileMenu);
    m_ClearCacheAction = new QAction("Clear Cache", m_FileMenu);
    m_CloseAction = new QAction("Close Player", m_FileMenu);
    m_CloseAction->setShortcut(QKeySequence("Ctrl+Q"));

    m_FileMenu->addAction(m_OpenAction);
    m_FileMenu->addAction(m_ClearAction);
    m_FileMenu->addAction(m_ClearCacheAction);
    m_FileMenu->addAction(m_CloseAction);

    menuBar->addMenu(m_FileMenu);

    setMenuBar(menuBar);
}

void VoidMainWindow::Connect()
{
    /* Menu Actions */
    connect(m_CloseAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_OpenAction, SIGNAL(triggered()), this, SLOT(Load()));

    connect(m_ClearAction, SIGNAL(triggered()), m_Player, SLOT(Clear()));
    connect(m_ClearCacheAction, &QAction::triggered, this, &VoidMainWindow::ClearLookAheadCache);

    /* Media Lister */
    connect(m_MediaLister, &VoidMediaLister::mediaChanged, this, &VoidMainWindow::SetMedia);
    connect(m_MediaLister, &VoidMediaLister::mediaDropped, this, &VoidMainWindow::ReadDirectory);
}

void VoidMainWindow::CacheLookAhead()
{
    /* Check if we're supposed to cache look ahead for the media */
    if (m_CacheMedia)
    {
        /*
         * Get the current media to cache frames on a thread
         * Any frame which gets clicked on, in the timeslider caches the frame if not cached
         * meanwhile this cache continues on
         */
        std::thread t(&Media::Cache, m_Media);
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

void VoidMainWindow::ReadDirectory(const std::string& path)
{
    /* Update the media from the path */
    m_Media.Read(path);

    /* Cache */
    CacheLookAhead();

    /* Set the sequence on the Player */
    m_Player->Load(m_Media);

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
    ReadDirectory(mediaBrowser.GetDirectory());
}

void VoidMainWindow::SetMedia(const Media& media)
{
    /* Clear the player */
    m_Player->Clear();

    /* Update the sequence */
    m_Media = media;

    /* Set the sequence on the Player */
    m_Player->Load(m_Media);
}

VOID_NAMESPACE_CLOSE
