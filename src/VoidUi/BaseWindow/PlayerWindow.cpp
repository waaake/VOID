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
    RegisterDocks();
    Build();

    setWindowTitle("VOID");
    setAttribute(Qt::WA_QuitOnClose);

    /* Connect Signals -> Slots */
    Connect();
}

VoidMainWindow::~VoidMainWindow()
{
    m_MediaLister->deleteLater();
    m_PlayLister->deleteLater();
    m_ScriptEditor->deleteLater();
    m_MetadataViewer->deleteLater();
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
    connect(m_MediaLister, &VoidMediaLister::metadataInspected, this, &VoidMainWindow::InspectMetadata);
}

void VoidMainWindow::RegisterDocks()
{
    /* Register Dock Widgets in the Dock Manager */
    DockManager& manager = DockManager::Instance();

    /* Media Lister Widget */
    m_MediaLister = new VoidMediaLister(this);
    m_PlayLister = new VoidPlayLister();

    /* Python Script Editor */
    m_ScriptEditor = new PyScriptEditor();

    /* Media Metadata Viewer */
    m_MetadataViewer = new MetadataViewer();

    manager.RegisterDock(m_MediaLister, "Media View");
    manager.RegisterDock(_PlayerBridge.ActivePlayer(), "Viewer");
    manager.RegisterDock(m_ScriptEditor, "Script Editor");
    manager.RegisterDock(m_MetadataViewer, "Metadata Viewer");
    manager.RegisterDock(m_PlayLister, "Playlist View");
}

void VoidMainWindow::InitMenu(MenuSystem* menuSystem)
{
    /* File Menu {{{ */
    QMenu* fileMenu = menuSystem->AddMenu("File");

    QAction* importAction = menuSystem->AddAction(fileMenu, "Import Media...", QKeySequence("Ctrl+I"));
    QAction* importDirectoryAction = menuSystem->AddAction(fileMenu, "Import Directory...", QKeySequence("Ctrl+Alt+I"));

    /* -------------------------------- */
    fileMenu->addSeparator();

    QAction* newProjectAction = menuSystem->AddAction(fileMenu, "New Project", QKeySequence("Ctrl+N"));
    QAction* saveProjectAction = menuSystem->AddAction(fileMenu, "Save Project", QKeySequence("Ctrl+S"));
    QAction* saveAsProjectAction = menuSystem->AddAction(fileMenu, "Save Project As...", QKeySequence("Ctrl+Shift+S"));
    QAction* loadProjectAction = menuSystem->AddAction(fileMenu, "Open Project", QKeySequence("Ctrl+O"));
    QAction* closeProjectAction = menuSystem->AddAction(fileMenu, "Close Project", QKeySequence("Ctrl+W"));

    /* -------------------------------- */
    fileMenu->addSeparator();

    QAction* clearAction = menuSystem->AddAction(fileMenu, "Clear Viewer");
    QAction* closeAction = menuSystem->AddAction(fileMenu, "Quit VOID", QKeySequence("Ctrl+Q"));

    connect(importAction, &QAction::triggered, this, []() -> void { _ProjectBridge.ImportMedia(); });
    connect(importDirectoryAction, &QAction::triggered, this, []() -> void { _ProjectBridge.ImportDirectory(); });
    connect(newProjectAction, &QAction::triggered, this, [this]() { _MediaBridge.NewProject(); });
    connect(saveProjectAction, &QAction::triggered, this, []() -> void { _ProjectBridge.Save(); });
    connect(saveAsProjectAction, &QAction::triggered, this, []() -> void { _ProjectBridge.SaveAs(); });
    connect(loadProjectAction, &QAction::triggered, this, []() -> void { _ProjectBridge.Open(); });
    connect(closeProjectAction, &QAction::triggered, this, []() -> void { _ProjectBridge.Close(); });
    connect(clearAction, &QAction::triggered, &_PlayerBridge, &PlayerBridge::Clear);
    connect(closeAction, &QAction::triggered, this, &QCoreApplication::quit);
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

    /* Player Menu(s) {{{ */
    _PlayerBridge.InitMenu(menuSystem);
    /* }}} */

    /* Help Menu {{{ */
    QMenu* helpMenu = menuSystem->AddMenu("Help");
    QAction* aboutAction = menuSystem->AddAction(helpMenu, "About VOID");

    connect(aboutAction, &QAction::triggered, this, [this]() { AboutVoid(this).exec(); });
    /* }}} */
}

void VoidMainWindow::InspectMetadata(const SharedMediaClip& media)
{
    m_MetadataViewer->SetFromMedia(media);
}

VOID_NAMESPACE_CLOSE
