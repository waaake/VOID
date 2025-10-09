// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYER_WINDOW_H
#define _VOID_PLAYER_WINDOW_H

/* Qt */
#include <QMainWindow>
#include <QList>

/* Internal */
#include "About.h"
#include "Definition.h"
#include "BaseWindow.h"
#include "TitleBar.h"
#include "MenuSystem.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidUi/Dock/Docker.h"
#include "VoidUi/Player/Player.h"
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

    void InitMenu(MenuSystem* menuSystem);
    void PlayMedia(const std::vector<SharedMediaClip>& items);

    /* Inspect Media Information */
    void InspectMetadata(const SharedMediaClip& media);

    SharedPlaybackSequence ActiveSequence() const { return m_Sequence; }
    Player* ActivePlayer() const { return m_Player; }
    MetadataViewer* GetMetadataViewer() const { return m_MetadataViewer; }
    QMenuBar* MenuBar() const;

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
