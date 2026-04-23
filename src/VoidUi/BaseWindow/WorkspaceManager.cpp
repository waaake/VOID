// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "WorkspaceManager.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

WorkspaceManager::WorkspaceManager(QWidget* parent)
    : MainWindow(parent)
    , m_Current(Workspace::BASIC)
{
}

WorkspaceManager::~WorkspaceManager()
{
    m_MediaLister->deleteLater();
    m_PlayLister->deleteLater();
    m_ScriptEditor->deleteLater();
    m_MetadataViewer->deleteLater();
}

QWidget* WorkspaceManager::Widget(const Component& component) const
{
    return DockManager::Instance().Dock(static_cast<int>(component)).widget;
}

void WorkspaceManager::Init()
{
    // Register Dock Widgets in the Dock Manager
    DockManager& manager = DockManager::Instance();

    // Media Lister Widget
    m_MediaLister = new VoidMediaLister();
    m_PlayLister = new VoidPlayLister();

    // Python Script Editor
    m_ScriptEditor = new PyScriptEditor();

    // Media Metadata Viewer
    m_MetadataViewer = new MetadataViewer();

    // Properties Editor
    m_PropertiesEditor = new PropertiesPanel();

    manager.RegisterDock(m_MediaLister, "Media View");
    manager.RegisterDock(_PlayerBridge.ActivePlayer(), "Viewer");
    manager.RegisterDock(m_ScriptEditor, "Script Editor");
    manager.RegisterDock(m_MetadataViewer, "Metadata Viewer");
    manager.RegisterDock(m_PlayLister, "Playlist View");
    manager.RegisterDock(m_PropertiesEditor, "Properties");

    // Docker
    m_Splitter = new DockSplitter(Qt::Horizontal, this);
    setCentralWidget(m_Splitter);
}

void WorkspaceManager::Connect()
{
    connect(m_MediaLister, &VoidMediaLister::effectsEdited, this, &WorkspaceManager::EditEffects);
    connect(m_MediaLister, &VoidMediaLister::metadataInspected, this, &WorkspaceManager::InspectMetadata);
    connect(_PlayerBridge.ActivePlayer(), &Player::metadataInspected, this, &WorkspaceManager::InspectMetadata);
}

void WorkspaceManager::InitMenu(MenuSystem* menuSystem)
{
    QMenu* workspaceMenu = menuSystem->AddMenu("Workspace");

    QAction* playbackWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Switch to Playback Workspace", QKeySequence("Shift+F1"));
    QAction* basicWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Switch to Basic Workspace", QKeySequence("Shift+F2"));
    QAction* reviewWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Switch to Review Workspace", QKeySequence("Shift+F3"));
    QAction* scriptingWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Switch to Scripting Workspace", QKeySequence("Shift+F4"));
    QAction* editingWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Switch to Editing Workspace", QKeySequence("Shift+F5"));

    workspaceMenu->addSeparator();

    QAction* setCurrentWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Set current workspace as default");
    QAction* resetDefaultWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Reset the default workspace");

    connect(playbackWorkspaceAction, &QAction::triggered, this, [&]() -> void { Switch(Workspace::PLAYBACK); });
    connect(basicWorkspaceAction, &QAction::triggered, this, [&]() -> void { Switch(Workspace::BASIC); });
    connect(reviewWorkspaceAction, &QAction::triggered, this, [&]() -> void { Switch(Workspace::REVIEW); });
    connect(scriptingWorkspaceAction, &QAction::triggered, this, [&]() -> void { Switch(Workspace::SCRIPTING); });
    connect(editingWorkspaceAction, &QAction::triggered, this, [&]() -> void { Switch(Workspace::EDITING); });

    connect(setCurrentWorkspaceAction, &QAction::triggered, this, [&]() -> void
    {
        VoidPreferences::Instance().Set(Settings::DefaultWorkspace, static_cast<int>(m_Current));
    });

    connect(resetDefaultWorkspaceAction, &QAction::triggered, this, [&]() -> void
    {
        VoidPreferences::Instance().Set(Settings::DefaultWorkspace, 0);
        Switch(Workspace::PLAYBACK);
    });
}

void WorkspaceManager::Switch(const Workspace& workspace)
{
    m_Splitter->ClearPanes();
    m_Current = workspace;

    switch (workspace)
    {
        case Workspace::BASIC:
            m_Splitter->AddPane(static_cast<int>(Component::Viewer));
            break;
        case Workspace::REVIEW:
            m_Splitter->AddSplitPane(
                static_cast<int>(Component::MediaLister),
                static_cast<int>(Component::PlayLister),
                Qt::Vertical
            );
            m_Splitter->AddPane(static_cast<int>(Component::Viewer));
            break;
        case Workspace::SCRIPTING:
            m_Splitter->AddSplitPane(
                static_cast<int>(Component::MediaLister),
                static_cast<int>(Component::ScriptEditor),
                Qt::Vertical
            );
            m_Splitter->AddPane(static_cast<int>(Component::Viewer));
            break;
        case Workspace::EDITING:
            m_Splitter->AddPane(static_cast<int>(Component::MediaLister));
            m_Splitter->AddPane(static_cast<int>(Component::Viewer));
            m_Splitter->AddPane(static_cast<int>(Component::Properties));
            break;
        case Workspace::PLAYBACK:
        default:
            m_Splitter->AddPane(static_cast<int>(Component::MediaLister));
            m_Splitter->AddPane(static_cast<int>(Component::Viewer));
    }
}

void WorkspaceManager::Clear()
{
    m_Splitter->ClearPanes();
}

void WorkspaceManager::InspectMetadata(const SharedMediaClip& media)
{
    m_MetadataViewer->SetFromMedia(media);

    // Show if it's not already being shown
    const DockStruct d = DockManager::Instance().Dock(static_cast<int>(Component::MetadataViewer));
    if (!d.widget->isVisible())
        ShowComponent(Component::MetadataViewer);
}

void WorkspaceManager::EditEffects(const SharedMediaClip& media)
{
    for (auto effect : media->Effects())
        m_PropertiesEditor->EditEffect(effect);

    const DockStruct d = DockManager::Instance().Dock(static_cast<int>(Component::Properties));
    if (!d.widget->isVisible())
        ShowComponent(Component::Properties);
}

void WorkspaceManager::ShowComponent(const Component& component) const
{
    MainWindow* window = new MainWindow;
	DockSplitter* splitter = new DockSplitter(Qt::Horizontal, window);
	DockWidget* undocked = new DockWidget(splitter, true);
	window->setCentralWidget(undocked);

	undocked->AddDockManagerWidget(static_cast<int>(component));
	window->show();
}

VOID_NAMESPACE_CLOSE
