// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAY_LISTER_H
#define _VOID_PLAY_LISTER_H

/* Qt */
#include <QButtonGroup>
#include <QLayout>
#include <QSplitter>
#include <QWidget>

/* Internal */
#include "QDefinition.h"
#include "VoidQExtensions/PushButton.h"
#include "VoidMediaPlayer/Media/MediaSearchBar.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"
#include "VoidMediaPlayer/Playlist/Views/PlaylistView.h"
#include "VoidMediaPlayer/Playlist/Views/PlaylistMediaView.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidPlayLister : public QWidget
{
    Q_OBJECT
public:
    VoidPlayLister(QWidget* parent = nullptr);
    virtual ~VoidPlayLister();

    inline QSize sizeHint() const override { return QSize(300, 720); }

signals:
    void playlistChanged(const std::vector<SharedMediaClip>&);
    void mediaChanged(const SharedMediaClip&);

protected: /* Methods */
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
    void SetFromPreferences();

    void AddSelectionToSequence();
    void RemoveSelectedMedia();

    void IndexSelected(const QModelIndex& index);

    void Play(Playlist* playlist);
    void PlayAsSequence(Playlist* playlist);
    void Play(const std::vector<SharedMediaClip>& media);

private: /* Members */
    QVBoxLayout* m_layout;
    QHBoxLayout* m_OptionsLayout;
    QSplitter* m_ViewSplitter;

    HighlightToggleButton* m_ListViewToggle;
    HighlightToggleButton* m_DetailedListViewToggle;
    HighlightToggleButton* m_ThumbnailViewToggle;

    QButtonGroup* m_ViewButtonGroup;
    MediaSearchBar* m_SearchBar;

    PlaylistView* m_PlaylistView;
    PlaylistMediaView* m_MediaView;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAY_LISTER_H
