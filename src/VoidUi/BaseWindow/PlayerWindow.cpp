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
#include "VoidUi/BaseWindow/StartupWindow.h"

VOID_NAMESPACE_OPEN

VoidMainWindow::VoidMainWindow(QWidget* parent)
    : BaseWindow(parent)
    , m_Bridge(MBridge::Instance())
{
    Build();

    setWindowTitle("VOID");
    setAttribute(Qt::WA_QuitOnClose);

    /* Connect Signals -> Slots */
    Connect();
}

VoidMainWindow::~VoidMainWindow()
{
    m_WorkspaceManager->deleteLater();
    delete m_WorkspaceManager;
    m_WorkspaceManager = nullptr;
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
    m_WorkspaceManager = new WorkspaceManager(this);
    m_WorkspaceManager->Init();

    layout->addWidget(m_WorkspaceManager);

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

    m_WorkspaceManager->Connect();
}

void VoidMainWindow::SwitchWorkspace(const Workspace& workspace)
{
    m_WorkspaceManager->Switch(workspace);
}

void VoidMainWindow::InitMenu(MenuSystem* menuSystem)
{
    /* File Menu {{{ */
    QMenu* fileMenu = menuSystem->AddMenu("File");

    QAction* openMediaAction = menuSystem->AddAction(fileMenu, "Open Media...");
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

    fileMenu->addMenu(_MediaBridge.RecentProjectsMenu(fileMenu));

    /* -------------------------------- */
    fileMenu->addSeparator();

    QAction* clearAction = menuSystem->AddAction(fileMenu, "Clear Viewer");
    QAction* closeAction = menuSystem->AddAction(fileMenu, "Close VOID", QKeySequence("Ctrl+Q"));

    connect(openMediaAction, &QAction::triggered, this, []() -> void
    {
        if (const SharedMediaClip& media = _ProjectBridge.OpenMedia())
        {
            /**
             * TODO: The player currently faces a race condition when caching media and playing
             * the first frame, there's mostly a case where the frame is read but not cached completely
             * at the moment, calling refresh (re-render the current frame) works to load the first frame
             * of the media, but this needs some change, possibly on the way how media cache is handled 
             */
            _PlayerBridge.SetMedia(media);
            _PlayerBridge.Refresh();
        }
    });
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
    icon.addPixmap(IconForge::GetPixmap(IconType::icon_build, _DARK_COLOR(QPalette::Text, 100), 18), QIcon::Normal);
    icon.addPixmap(IconForge::GetPixmap(IconType::icon_build, _COLOR(QPalette::Dark), 18), QIcon::Active);
    editPrefsAction->setIcon(icon);

    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);

    /* -------------------------------- */
    editMenu->addSeparator();

    editMenu->addAction(editPrefsAction);

    connect(editPrefsAction, &QAction::triggered, this, [this]() { VoidPreferencesWidget(this).exec(); } );
    /* }}} */

    /* Workspace Menu {{{ */
    m_WorkspaceManager->InitMenu(menuSystem);
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

VOID_NAMESPACE_CLOSE
