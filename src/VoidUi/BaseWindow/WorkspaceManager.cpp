// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "WorkspaceManager.h"

VOID_NAMESPACE_OPEN

WorkspaceManager::WorkspaceManager(QWidget* parent)
    : QMainWindow(parent)
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
    /* Register Dock Widgets in the Dock Manager */
    DockManager& manager = DockManager::Instance();

    /* Media Lister Widget */
    m_MediaLister = new VoidMediaLister();
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

    /* Docker */
    m_Splitter = new DockSplitter(Qt::Horizontal, this);
    setCentralWidget(m_Splitter);
}

void WorkspaceManager::Connect()
{
    connect(m_MediaLister, &VoidMediaLister::metadataInspected, this, &WorkspaceManager::InspectMetadata);
}

void WorkspaceManager::InitMenu(MenuSystem* menuSystem)
{
    QMenu* workspaceMenu = menuSystem->AddMenu("Workspace");

    QAction* basicWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Switch to Basic Workspace");
    QAction* playbackWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Switch to Playback Workspace");
    QAction* reviewWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Switch to Review Workspace");
    QAction* scriptingWorkspaceAction = menuSystem->AddAction(workspaceMenu, "Switch to Scripting Workspace");

    connect(basicWorkspaceAction, &QAction::triggered, this, [this]() { Switch(Workspace::BASIC); });
    connect(playbackWorkspaceAction, &QAction::triggered, this, [this]() { Switch(Workspace::PLAYBACK); });
    connect(reviewWorkspaceAction, &QAction::triggered, this, [this]() { Switch(Workspace::REVIEW); });
    connect(scriptingWorkspaceAction, &QAction::triggered, this, [this]() { Switch(Workspace::SCRIPTING); });
}

void WorkspaceManager::Switch(const Workspace& workspace)
{
    m_Splitter->ClearPanes();

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
}

VOID_NAMESPACE_CLOSE
