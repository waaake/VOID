// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_LISTER_H
#define _VOID_MEDIA_LISTER_H

/* STD */
#include <vector>
#include <unordered_map>

/* Qt */
#include <QAction>
#include <QButtonGroup>
#include <QSlider>
#include <QLayout>
#include <QMenu>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QSplitter>
#include <QWidget>

/* Internal */
#include "QDefinition.h"
#include "MediaSearchBar.h"
#include "VoidMediaPlayer/Media/Views/MediaView.h"
#include "VoidMediaPlayer/Project/Views/ProjectView.h"
#include "VoidQExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidMediaLister : public QWidget
{
    Q_OBJECT
public:
    VoidMediaLister(QWidget* parent = nullptr);
    virtual ~VoidMediaLister();

    QSize sizeHint() const override { return QSize(300, 720); }

signals:
    void mediaChanged(const SharedMediaClip&);
    // For a bunch of media is set to be played
    void playlistChanged(const std::vector<SharedMediaClip>&);
    void mediaDropped(const std::string&);
    // When a Media is inspected of its Metdata
    void metadataInspected(const SharedMediaClip&);
    void effectsEdited(const SharedMediaClip&);

protected: /* Methods */
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private: /* Methods */
    void Build();

    /* Setup how the default UI elements appear */
    void Setup();

    /* Connects Signals across the componets of the widget */
    void Connect();

    /* Show Context Menu at the given position */
    void ShowContextMenu(const _QPoint& position);
    void RescaleThumbnails(float scale);

    void AddSelectionToSequence();
    void PlaySelectionAsQueue();
    void AddSelectionToQueue();
    void PlaySelectionAsGrid();
    // void RemoveSelectedMedia();
    void InspectMetadata();
    void EditEffects();

    void IndexSelected(const QModelIndex& index);

private: /* Members */
    QVBoxLayout* m_layout;
    QHBoxLayout* m_OptionsLayout;
    QSplitter* m_ViewSplitter;

    HighlightToggleButton* m_ListViewToggle;
    HighlightToggleButton* m_DetailedListViewToggle;
    HighlightToggleButton* m_ThumbnailViewToggle;

    QButtonGroup* m_ViewButtonGroup;

    MediaSearchBar* m_SearchBar;
    QSlider* m_ScaleSlider;
    HighlightToggleButton* m_SortButton;

    ProjectView* m_ProjectView;
    MediaView* m_MediaView;

    QMenu* m_PlayMenu;
    QMenu* m_SequenceMenu;
    QMenu* m_TagsMenu;

    QAction* m_PlayAction;
    QAction* m_PlayAsListAction;
    QAction* m_AddToQueueAction;
    QAction* m_PlayAsGridAction;
    QAction* m_RemoveAction;
    QAction* m_InspectMetadataAction;
    QAction* m_AddTagAction;
    QAction* m_ClearTagsAction;
    QMenu* m_PlaylistMenu;
    QAction* m_CreatePlaylistAction;
    QAction* m_AddSequenceAction;
    QAction* m_SaveSnapshotAction;
    QAction* m_RestoreSnapshotAction;
    QAction* m_RenameAction;

    QShortcut* m_PrimaryViewShortcut;
    QShortcut* m_SecondaryViewShortcut;

private: /* Methods */
    void SetFromPreferences();
    // void ProjectChanged();
    void RebuildPlaylistMenu();
    void AddToPrimaryViewer() const;
    void AddToSecondaryViewer() const;
    void AddSelectionToPlaylist(Playlist* playlist) const;
    void AddTagToSelected();
    void EditSelectedTags(const QModelIndex& index, const QPoint& position);
    void ClearTagsFromSelected();
    bool IsSequenceSelected() const;
    bool HasSnapshots() const;
    void SaveSnapshot();
    void RestoreSnapshot();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_LISTER_H
