// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_WORKSPACE_MANAGER_H
#define _VOID_WORKSPACE_MANAGER_H

/* Internal */
#include "Definition.h"
#include "MenuSystem.h"
#include "VoidUi/Dock/DockManager.h"
#include "VoidUi/Dock/Docker.h"

/* Internal Widget Components */
#include "VoidUi/Editor/Properties.h"
#include "VoidUi/Media/MediaLister.h"
#include "VoidUi/Media/MetadataViewer.h"
#include "VoidUi/Player/PlayerBridge.h"
#include "VoidUi/Playlist/PlayLister.h"
#include "VoidUi/ScriptEditor/ScriptEditor.h"
#include "VoidUi/QExtensions/Window.h"
#include "VoidUi/Media/MediaQueue.h"

VOID_NAMESPACE_OPEN

enum class Workspace
{
    PLAYBACK,
    BASIC,
    REVIEW,
    SCRIPTING,
    EDITING
};

/**
 * This enum follows the ordering with which the Widget Components
 * are registered in the DockManager hence getting the correspoding IDs
 * like MediaLister gets id 0, viewer gets 1 and so on...
 * 
 * Then this enum could be used in place of the IDs
 */
enum class Component
{
    MediaLister,
    Viewer,
    ScriptEditor,
    MetadataViewer,
    PlayLister,
    Properties,
    MediaQueue,
};

class WorkspaceManager : public MainWindow
{
public:
    WorkspaceManager(QWidget* parent = nullptr);
    ~WorkspaceManager();

    void Init();
    void Connect();
    void InitMenu(MenuSystem* menuSystem);
    void Switch(const Workspace& workspace = Workspace::PLAYBACK);

    /**
     * Returns any component Widget which is part of the Dock Setup
     */
    QWidget* Widget(const Component& component) const;

private: /* Members */
    DockSplitter* m_Splitter;

    VoidMediaLister* m_MediaLister;
    VoidPlayLister* m_PlayLister;
    PyScriptEditor* m_ScriptEditor;
    MetadataViewer* m_MetadataViewer;
    PropertiesPanel* m_PropertiesEditor;
    MediaQueue* m_MediaQueue;

    Workspace m_Current;

private: /* Members */
    void Clear();
    void InspectMetadata(const SharedMediaClip& media);
    void UpdateMediaQueue(Playlist* playlist);
    void EditEffects(const SharedMediaClip& media);
    void ShowComponent(const Component& component) const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_WORKSPACE_MANAGER_H
