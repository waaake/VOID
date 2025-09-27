// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYER_WINDOW_H
#define _VOID_PLAYER_WINDOW_H

/* Qt */
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QList>

/* Internal */
#include "About.h"
#include "Definition.h"
#include "BaseWindow.h"
#include "TitleBar.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidUi/Dock/Docker.h"
#include "VoidUi/Player/PlayerWidget.h"
#include "VoidUi/Media/MediaLister.h"
#include "VoidUi/Playlist/PlayLister.h"
#include "VoidUi/Media/MetadataViewer.h"
#include "VoidUi/ScriptEditor/ScriptEditor.h"

VOID_NAMESPACE_OPEN

class DockerWindow : public QMainWindow
{
public: /* Enum */
    enum class Component
    {
        MediaLister,
        Viewer,
        ScriptEditor
    };

public:
    explicit DockerWindow(QWidget* parent = nullptr);
    virtual ~DockerWindow();

    void ToggleComponent(const Component& component, const bool state);

private: /* Members */
    QList<int> m_DockSizes;
    DockSplitter* m_Splitter;

private: /* Methods */
    void Build();
    void ToggleDock(VoidDocker* dock, const bool state, const Qt::DockWidgetArea& area);
};

class VOID_API VoidMainWindow : public BaseWindow
{
    Q_OBJECT

public:
    VoidMainWindow(QWidget* parent = nullptr);
    virtual ~VoidMainWindow();

    virtual QSize sizeHint() const override;

    void PlayMedia(const std::vector<SharedMediaClip>& items);

    /* Inspect Media Information */
    void InspectMetadata(const SharedMediaClip& media);

    SharedPlaybackSequence ActiveSequence() const { return m_Sequence; }
    Player* ActivePlayer() const { return m_Player; }
    MetadataViewer* GetMetadataViewer() const { return m_MetadataViewer; }

private: /* Methods */
    void Build();
    void Connect();

    /* Register Internal components as Dock Widgets */
    void RegisterDocks();

protected:
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    Player* m_Player;
    VoidMediaLister* m_MediaLister;
    VoidPlayLister* m_PlayLister;
    PyScriptEditor* m_ScriptEditor;
    MetadataViewer* m_MetadataViewer;

    DockerWindow* m_InternalDocker;

    VoidTitleBar* m_TitleBar;

    /* Window Menu */
    QLabel* m_VoidMenuIcon;

    /* File Menu */
    QMenu* m_FileMenu;
    QAction* m_ImportAction;
    QAction* m_ImportDirectoryAction;
    QAction* m_ClearAction;
    QAction* m_NewProjectAction;
    QAction* m_SaveProjectAction;
    QAction* m_SaveAsProjectAction;
    QAction* m_LoadProjectAction;
    QAction* m_CloseProjectAction;
    QAction* m_CloseAction;

    /* Edit Menu */
    QMenu* m_EditMenu;
    QAction* m_UndoAction;
    QAction* m_RedoAction;
    QAction* m_EditPrefsAction;
    
    /* Playback Menu */
    QMenu* m_PlaybackMenu;
    QAction* m_EnableCacheAction;
    QAction* m_DisableCacheAction;
    QAction* m_StopCacheAction;
    QAction* m_ClearCacheAction;
    QAction* m_RefreshCacheAction;
    QAction* m_ResumeCacheAction;
    QAction* m_PlayForwardsAction;
    QAction* m_StopPlayingAction;
    QAction* m_PlayBackwardsAction;
    QAction* m_ForwardsAction;
    QAction* m_BackwardsAction;
    QAction* m_EndFrameAction;
    QAction* m_StartFrameAction;
    QAction* m_SetInFrameAction;
    QAction* m_SetOutFrameAction;
    QAction* m_ResetRangeAction;

    /* Viewer Menu */
    QMenu* m_ViewerMenu;
    QAction* m_ZoomInAction;
    QAction* m_ZoomOutAction;
    QAction* m_ZoomToFitAction;
    QAction* m_FullscreenAction;
    QAction* m_ExitFullscreenAction;

    /* Window Menu */
    QMenu* m_WindowMenu;
    QAction* m_MediaListerAction;

    /* Help Menu */
    QMenu* m_HelpMenu;
    QAction* m_AboutAction;

    /* Media Bridge Instance */
    MBridge& m_Bridge;

    /* Playback Sequence holding Media entities internally */
    SharedPlaybackSequence m_Sequence;
    /* The track which gets used on the Sequence */
    SharedPlaybackTrack m_Track;

public:
    /* Clears and sets the provided media on the player */
    void SetMedia(const SharedMediaClip& media);
    /* Adds media onto the existing track */
    void AddMedia(const SharedMediaClip& media);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WINDOW_H
