// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYER_WINDOW_H
#define _VOID_PLAYER_WINDOW_H

/* Qt */
#include <QMainWindow>

/* Internal */
#include "Definition.h"
#include "BaseWindow.h"
#include "TitleBar.h"
#include "MenuSystem.h"
#include "WorkspaceManager.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidMainWindow : public BaseWindow
{
    Q_OBJECT

public:
    VoidMainWindow(QWidget* parent = nullptr);
    virtual ~VoidMainWindow();

    /**
     * @brief Initializes the Menu for the Main Player.
     * 
     * @param menuSystem Menu system pointer for Menu creation.
     */
    void InitMenu(MenuSystem* menuSystem);

    inline Player* ActivePlayer() const { return _PlayerBridge.ActivePlayer(); }
    MetadataViewer* GetMetadataViewer() const { return dynamic_cast<MetadataViewer*>(m_WorkspaceManager->Widget(Component::MetadataViewer)); }
    QMenuBar* MenuBar() const;
    void SwitchWorkspace(const Workspace& workspace = Workspace::PLAYBACK);

private: /* Methods */
    void Build();
    void Connect();

protected:
    inline QSize sizeHint() const override { return QSize(1280, 760); }
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    VoidTitleBar* m_TitleBar;
    WorkspaceManager* m_WorkspaceManager;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WINDOW_H
