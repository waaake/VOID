// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAY_LISTER_H
#define _VOID_PLAY_LISTER_H

/* Qt */
#include <QLayout>
#include <QPushButton>
#include <QShortcut>
#include <QSplitter>
#include <QWidget>

/* Internal */
#include "QDefinition.h"
#include "VoidUi/Media/MediaSearchBar.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Playlist/Views/PlaylistView.h"
#include "VoidUi/Playlist/Views/PlaylistMediaView.h"

VOID_NAMESPACE_OPEN

class VoidPlayLister : public QWidget
{
    Q_OBJECT

public:
    VoidPlayLister(QWidget* parent = nullptr);
    virtual ~VoidPlayLister();

    /* Override the default size of the widget */
    QSize sizeHint() const override;

signals:
    /* For a bunch of media is set to be played */
    void playlistChanged(const std::vector<SharedMediaClip>&);
    void mediaChanged(const SharedMediaClip&);

protected: /* Methods */
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private: /* Methods */
    void Build();

    /* Setup how the default UI elements appear */
    void Setup();

    /* Connects Signals across the componets of the widget */
    void Connect();

    void AddSelectionToSequence();
    void RemoveSelectedMedia();

    void IndexSelected(const QModelIndex& index);

    void Play(const Playlist* playlist);
    void Play(const std::vector<SharedMediaClip>& media);

private: /* Members */
    QVBoxLayout* m_layout;
    QHBoxLayout* m_OptionsLayout;
    QSplitter* m_ViewSplitter;

    MediaSearchBar* m_SearchBar;

    /* Options */
    QPushButton* m_CreateButton;
    QPushButton* m_DeleteButton;

    /* Views */
    PlaylistView* m_PlaylistView;
    PlaylistMediaView* m_MediaView;

    /* Shortcuts */
    QShortcut* m_DeleteShortcut;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAY_LISTER_H
