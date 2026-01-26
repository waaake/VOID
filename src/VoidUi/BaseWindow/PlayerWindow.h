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
#include "WorkspaceManager.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidUi/Dock/Docker.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidMainWindow : public BaseWindow
{
    Q_OBJECT

public:
    VoidMainWindow(QWidget* parent = nullptr);
    virtual ~VoidMainWindow();

    void InitMenu(MenuSystem* menuSystem);

    inline Player* ActivePlayer() const { return _PlayerBridge.ActivePlayer(); }
    MetadataViewer* GetMetadataViewer() const { return dynamic_cast<MetadataViewer*>(m_WorkspaceManager->Widget(Component::MetadataViewer)); }
    QMenuBar* MenuBar() const;
    void SwitchWorkspace(const Workspace& workspace = Workspace::PLAYBACK);

private: /* Methods */
    void Build();
    void Connect();

protected:
    virtual QSize sizeHint() const override;
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event);

private: /* Members */
    VoidTitleBar* m_TitleBar;
    WorkspaceManager* m_WorkspaceManager;

    /* Media Bridge Instance */
    MBridge& m_Bridge;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WINDOW_H
