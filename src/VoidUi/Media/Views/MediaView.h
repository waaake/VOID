// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_LIST_VIEW_H
#define _VOID_MEDIA_LIST_VIEW_H

/* Qt */
#include <QListView>

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Media/Delegates/ListDelegate.h"
#include "VoidUi/Media/Delegates/ThumbnailDelegate.h"

VOID_NAMESPACE_OPEN

class MediaView : public QListView
{
    Q_OBJECT

public: /* enums */

    enum class ViewType
    {
        DetailedListView,
        ListView,
        ThumbnailView
    };

public:
    explicit MediaView(QWidget* parent = nullptr);
    ~MediaView();

    /* Search and filter items from the Model */
    inline void Search(const std::string& text) { m_Proxy->SetSearchText(text); }

    /* Returns the currently selected Media row Model Indices */
    const std::vector<QModelIndex> SelectedIndexes() const;

    /* Returns if the widget has selection */
    bool HasSelection();

    /* Toggle sorting on the Model */
    void EnableSorting(bool state, const Qt::SortOrder& order = Qt::AscendingOrder);

    const ViewType GetViewType() const { return m_ViewType; }
    /* Set the View Type */
    void SetViewType(const ViewType& type);
    void RemoveSelectedMedia();

protected:
    void startDrag(Qt::DropActions supportedActions) override;

signals:
    /* Sends the Source Model Index mapped from the proxy model */
    void itemDoubleClicked(const QModelIndex&);
    void tagClicked(const QModelIndex&, const QPoint&);

private: /* Models */
    /* Proxy for filtering and sorting */
    MediaProxyModel* m_Proxy;

    BasicMediaItemDelegate* m_BasicDelegate;
    MediaItemDelegate* m_MediaDelegate;
    MediaThumbnailDelegate* m_ThumbnailDelegate;

    /* View Type for display */
    ViewType m_ViewType;

private: /* Methods */
    void Setup();
    void ResetView();
    void Connect();
    void ResetModel(MediaModel* model);
    void ItemDoubleClicked(const QModelIndex& index);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_LIST_VIEW_H
