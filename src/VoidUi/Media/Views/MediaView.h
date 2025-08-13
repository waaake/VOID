// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_LIST_VIEW_H
#define _VOID_MEDIA_LIST_VIEW_H

/* Qt */
#include <QListView>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

class MediaView : public QListView
{
    Q_OBJECT

public: /* enums */

    enum class ViewType
    {
        ListView,
        ThumbnailView
    };

public:
    explicit MediaView(QWidget* parent = nullptr);
    ~MediaView();

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

signals:
    /* Sends the Source Model Index mapped from the proxy model */
    void itemDoubleClicked(const QModelIndex&);

private: /* Models */
    /* Proxy for filtering and sorting */
    MediaProxyModel* proxy;

    /* View Type for display */
    ViewType m_ViewType;

private: /* Methods */
    /* Setup the View */
    void Setup();

    /* (Re)sets the view of the list */
    void ResetView();

    /* Setup Signals */
    void Connect();

    /* (Re)sets the Media Model */
    void ResetModel(MediaModel* model);

    /* Maps the Proxy Index to the source Model index before emitting */
    void ItemDoubleClicked(const QModelIndex& index);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_LIST_VIEW_H
