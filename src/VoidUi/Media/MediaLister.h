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
#include <QLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QSplitter>
#include <QWidget>

/* Internal */
#include "QDefinition.h"
#include "MediaBridge.h"
#include "MediaSearchBar.h"
#include "VoidUi/Media/Views/MediaView.h"
#include "VoidUi/Project/Views/ProjectView.h"
#include "VoidUi/QExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class VoidMediaLister : public QWidget
{
    Q_OBJECT

public:
    VoidMediaLister(QWidget* parent = nullptr);
    virtual ~VoidMediaLister();

    /* Override the default size of the widget */
    QSize sizeHint() const override;

signals:
    void mediaChanged(const SharedMediaClip& media);
    /* For a bunch of media is set to be played */
    void playlistChanged(const std::vector<SharedMediaClip>& media);
    void mediaDropped(const std::string& path);

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
    void ShowContextMenu(const Point& position);

    void AddSelectionToSequence();
    void RemoveSelectedMedia();

    void IndexSelected(const QModelIndex& index);

private: /* Members */
    QVBoxLayout* m_layout;
    QHBoxLayout* m_OptionsLayout;
    QSplitter* m_ViewSplitter;

    /* Options */
    HighlightToggleButton* m_ListViewToggle;
    HighlightToggleButton* m_ThumbnailViewToggle;

    /* Button Group to allow exclusive (single) selection */
    QButtonGroup* m_ViewButtonGroup;

    MediaSearchBar* m_SearchBar;
    HighlightToggleButton* m_SortButton;

    /* Views */
    ProjectView* m_ProjectView;
    MediaView* m_MediaView;

    /* Context Menu */
    QAction* m_PlayAction;
    QAction* m_RemoveAction;

    /* Shortcuts */
    QShortcut* m_DeleteShortcut;

private: /* Methods */
    /* Set values from User preferences */
    void SetFromPreferences();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_LISTER_H
