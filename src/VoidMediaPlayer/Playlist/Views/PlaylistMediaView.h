// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYLIST_MEDIA_LIST_VIEW_H
#define _VOID_PLAYLIST_MEDIA_LIST_VIEW_H

/* Qt */
#include <QAction>
#include <QListView>

/* Internal */
#include "QDefinition.h"
#include "VoidComponents/View.h"
#include "VoidObjects/Models/EntityModel.h"

VOID_NAMESPACE_OPEN

class PlaylistMediaView : public QListView, public IView
{
    Q_OBJECT
public:
    enum class ViewType
    {
        ListView,
        DetailedListView,
        ThumbnailView
    };
public:
    explicit PlaylistMediaView(QWidget* parent = nullptr);
    ~PlaylistMediaView();

    void DeleteSelected() override { RemoveSelected(); }

    // Search and filter items from the Model
    inline void Search(const std::string& text) { m_Proxy->SetSearchText(text); }

    // Returns the currently selected Media row Model Indices
    const std::vector<QModelIndex> SelectedIndexes() const;

    // Returns if the widget has selection
    bool HasSelection();

    // Toggle sorting on the Model
    void EnableSorting(bool state, const Qt::SortOrder& order = Qt::AscendingOrder);

    const ViewType GetViewType() const { return m_ViewType; }
    // Set the View Type
    void SetViewType(const ViewType& type);

    void Refresh();

protected:
    void startDrag(Qt::DropActions supportedActions) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
    void itemDoubleClicked(const QModelIndex&);
    void played(const std::vector<SharedMediaClip>&);

private: /* Models */
    EntityProxyModel* m_Proxy;
    ViewType m_ViewType;

    QAction* m_PlayAction;
    QAction* m_RemoveAction;

private: /* Methods */
    void Setup();
    void ResetView();
    void Connect();

    void ResetModel(EntityModel* model);
    void ItemDoubleClicked(const QModelIndex& index);
    void ShowContextMenu(const _QPoint& position);
    void PlaySelected();
    void RemoveSelected();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYLIST_MEDIA_LIST_VIEW_H
