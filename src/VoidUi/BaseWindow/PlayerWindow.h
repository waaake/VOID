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
#include "VoidUi/Dock/Docker.h"
#include "VoidUi/Player/PlayerBridge.h"
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

    void InitMenu(MenuSystem* menuSystem);

    /* Inspect Media Information */
    void InspectMetadata(const SharedMediaClip& media);

    Player* ActivePlayer() const { return _PlayerBridge.ActivePlayer(); }
    MetadataViewer* GetMetadataViewer() const { return m_MetadataViewer; }
    QMenuBar* MenuBar() const;

private: /* Methods */
    void Build();
    void Connect();

    /* Register Internal components as Dock Widgets */
    void RegisterDocks();

protected:
    virtual QSize sizeHint() const override;
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    VoidMediaLister* m_MediaLister;
    VoidPlayLister* m_PlayLister;
    PyScriptEditor* m_ScriptEditor;
    MetadataViewer* m_MetadataViewer;

    DockerWindow* m_InternalDocker;
    VoidTitleBar* m_TitleBar;

    /* Media Bridge Instance */
    MBridge& m_Bridge;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WINDOW_H
