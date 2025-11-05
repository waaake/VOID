// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_WORKSPACE_MANAGER_H
#define _VOID_WORKSPACE_MANAGER_H

/* Qt */
#include <QMainWindow>

/* Internal */
#include "Definition.h"
#include "MenuSystem.h"
#include "VoidUi/Dock/DockManager.h"
#include "VoidUi/Dock/Docker.h"

/* Internal Widget Components */
#include "VoidUi/Media/MediaLister.h"
#include "VoidUi/Media/MetadataViewer.h"
#include "VoidUi/Player/PlayerBridge.h"
#include "VoidUi/Playlist/PlayLister.h"
#include "VoidUi/ScriptEditor/ScriptEditor.h"

VOID_NAMESPACE_OPEN

enum class Workspace
{
    BASIC,
    PLAYBACK,
    REVIEW,
    SCRIPTING
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
};

class WorkspaceManager : public QMainWindow
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
    QMainWindow* m_Parent;
    DockSplitter* m_Splitter;

    VoidMediaLister* m_MediaLister;
    VoidPlayLister* m_PlayLister;
    PyScriptEditor* m_ScriptEditor;
    MetadataViewer* m_MetadataViewer;

private: /* Members */
    void Clear();
    void InspectMetadata(const SharedMediaClip& media);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_WORKSPACE_MANAGER_H
