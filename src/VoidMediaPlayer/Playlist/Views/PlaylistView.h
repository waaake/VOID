// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYLIST_VIEW_H
#define _VOID_PLAYLIST_VIEW_H

/* Qt */
#include <QAction>
#include <QListView>

/* Internal */
#include "QDefinition.h"
#include "VoidMediaPlayer/Project/Project.h"
#include "VoidObjects/Models/PlaylistModel.h"

VOID_NAMESPACE_OPEN

class PlaylistView : public QListView
{
    Q_OBJECT

public:
    explicit PlaylistView(QWidget* parent = nullptr);
    ~PlaylistView();

    // Search and filter items from the Model
    inline void Search(const std::string& text) { m_Proxy->SetSearchText(text); }

    // Returns the currently selected Project row Model Indices
    const std::vector<QModelIndex> SelectedIndexes() const;

    // Returns if the widget has selection
    bool HasSelection();

    // Toggle sorting on the Model
    void EnableSorting(bool state, const Qt::SortOrder& order = Qt::AscendingOrder);
    // Remove the selected playlist from the underlying model
    void RemoveSelected();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
    // Sends the Source Model Index mapped from the proxy model
    void itemClicked(const QModelIndex&);
    void played(Playlist*);
    void playedAsSequence(Playlist*);
    void updated();

private:
    PlaylistProxyModel* m_Proxy;

    QAction* m_PlayAction;
    QAction* m_PlayAsSequenceAction;
    QAction* m_RemoveAction;

private: /* Methods */
    void Setup();
    void Connect();
    void ResetModel(PlaylistModel* model);
    void ItemClicked(const QModelIndex& index);
    void ProjectChanged(const Project* project);
    void ShowContextMenu(const _QPoint& position);
    void Play();
    void PlayAsSequence();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYLIST_VIEW_H
