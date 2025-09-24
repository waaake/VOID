// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PROJECT_VIEW_H
#define _VOID_PROJECT_VIEW_H

/* Qt */
#include <QAction>
#include <QListView>

/* Internal */
#include "QDefinition.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

class ProjectView : public QListView
{
    Q_OBJECT

public:
    explicit ProjectView(QWidget* parent = nullptr);
    ~ProjectView();

    /* Search and filter items from the Model */
    inline void Search(const std::string& text) { proxy->SetSearchText(text); }

    /* Returns the currently selected Project row Model Indices */
    const std::vector<QModelIndex> SelectedIndexes() const;

    /* Returns if the widget has selection */
    bool HasSelection();

    /* Toggle sorting on the Model */
    void EnableSorting(bool state, const Qt::SortOrder& order = Qt::AscendingOrder);

signals:
    /* Sends the Source Model Index mapped from the proxy model */
    void itemClicked(const QModelIndex&);

private: /* Models */
    /* Proxy for filtering and sorting */
    ProjectProxyModel* proxy;

    QAction* m_ImportMediaAction;
    QAction* m_ImportDirectoryAction;
    QAction* m_CloseProjectAction;

private: /* Methods */
    /* Setup the View */
    void Setup();

    /* Setup Signals */
    void Connect();

    /* (Re)sets the Project Model */
    void ResetModel(ProjectModel* model);

    /* Maps the Proxy Index to the source Model index before emitting */
    void ItemClicked(const QModelIndex& index);

    void ShowContextMenu(const Point& position);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_VIEW_H
