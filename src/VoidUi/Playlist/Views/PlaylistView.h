// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYLIST_VIEW_H
#define _VOID_PLAYLIST_VIEW_H

/* Qt */
#include <QListView>

/* Internal */
#include "QDefinition.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidObjects/Models/PlaylistModel.h"

VOID_NAMESPACE_OPEN

class PlaylistView : public QListView
{
    Q_OBJECT

public:
    explicit PlaylistView(QWidget* parent = nullptr);
    ~PlaylistView();

    /* Search and filter items from the Model */
    inline void Search(const std::string& text) { proxy->SetSearchText(text); }

    /* Returns the currently selected Project row Model Indices */
    const std::vector<QModelIndex> SelectedIndexes() const;

    /* Returns if the widget has selection */
    bool HasSelection();

    /* Toggle sorting on the Model */
    void EnableSorting(bool state, const Qt::SortOrder& order = Qt::AscendingOrder);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
    /* Sends the Source Model Index mapped from the proxy model */
    void itemClicked(const QModelIndex&);

private: /* Models */
    /* Proxy for filtering and sorting */
    PlaylistProxyModel* proxy;

private: /* Methods */
    /* Setup the View */
    void Setup();

    /* Setup Signals */
    void Connect();

    /* (Re)sets the Playlist Model */
    void ResetModel(PlaylistModel* model);

    /* Maps the Proxy Index to the source Model index before emitting */
    void ItemClicked(const QModelIndex& index);

    void ProjectChanged(const Project* project);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYLIST_VIEW_H
