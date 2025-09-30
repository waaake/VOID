// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYLIST_MEDIA_LIST_VIEW_H
#define _VOID_PLAYLIST_MEDIA_LIST_VIEW_H

/* Qt */
#include <QAction>
#include <QListView>

/* Internal */
#include "QDefinition.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

class PlaylistMediaView : public QListView
{
    Q_OBJECT

public: /* enums */

    enum class ViewType
    {
        ListView,
        DetailedListView,
        ThumbnailView
    };

public:
    explicit PlaylistMediaView(QWidget* parent = nullptr);
    ~PlaylistMediaView();

    /* Search and filter items from the Model */
    inline void Search(const std::string& text) { proxy->SetSearchText(text); }

    /* Returns the currently selected Media row Model Indices */
    const std::vector<QModelIndex> SelectedIndexes() const;

    /* Returns if the widget has selection */
    bool HasSelection();

    /* Toggle sorting on the Model */
    void EnableSorting(bool state, const Qt::SortOrder& order = Qt::AscendingOrder);

    const ViewType GetViewType() const { return m_ViewType; }
    /* Set the View Type */
    void SetViewType(const ViewType& type);

    void Refresh();

protected:
    void startDrag(Qt::DropActions supportedActions) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
    /* Sends the Source Model Index mapped from the proxy model */
    void itemDoubleClicked(const QModelIndex&);
    void played(const std::vector<SharedMediaClip>&);

private: /* Models */
    /* Proxy for filtering and sorting */
    MediaProxyModel* proxy;

    /* View Type for display */
    ViewType m_ViewType;

    QAction* m_PlayAction;
    QAction* m_RemoveAction;

private: /* Methods */
    /* Setup the View */
    void Setup();

    /* (Re)sets the view of the list */
    void ResetView();

    /* Setup Signals */
    void Connect();

    void ResetModel(MediaModel* model);
    void ItemDoubleClicked(const QModelIndex& index);
    void ShowContextMenu(const Point& position);
    void PlaySelected();
    void RemoveSelected();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYLIST_MEDIA_LIST_VIEW_H
