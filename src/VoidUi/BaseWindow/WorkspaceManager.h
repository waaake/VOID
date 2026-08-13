// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_WORKSPACE_MANAGER_H
#define _VOID_WORKSPACE_MANAGER_H

/* Internal */
#include "Definition.h"
#include "VoidDocker/DockManager.h"
#include "VoidDocker/Docker.h"
#include "VoidMenuSystem/MenuSystem.h"
#include "VoidQExtensions/Window.h"

/* Internal Widget Components */
#include "VoidScriptEditor/ScriptEditor.h"
#include "VoidToolbox/Editor/Properties.h"
#include "VoidToolbox/Tools/TaskView.h"
#include "VoidUi/Media/MediaLister.h"
#include "VoidUi/Media/MetadataViewer.h"
#include "VoidUi/Player/PlayerBridge.h"
#include "VoidUi/Playlist/PlayLister.h"
#include "VoidUi/Media/MediaQueue.h"
#include "VoidUi/Sequencer/Sequencer.h"

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
    TaskQueue,
    Sequencer,
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

    void QueueTask(Task* task);

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
    TaskView* m_TaskQueue;
    SequencerTimeline* m_Sequencer;

    Workspace m_Current;

private: /* Members */
    void Clear();
    void InspectMetadata(const SharedMediaClip& media);
    void UpdateMediaQueue(Playlist* playlist);
    void EditEffects(const SharedMediaClip& media);
    void EditEffects(Effect* effect);
    void ShowComponent(const Component& component) const;
    bool ShowIfDocked(const QString& name) const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_WORKSPACE_MANAGER_H
