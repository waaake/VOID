// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ProjectView.h"
#include "VoidUi/Project/Delegates/ListDelegate.h"

VOID_NAMESPACE_OPEN

ProjectView::ProjectView(QWidget* parent)
    : QListView(parent)
{
    Setup();

    /* Connect Signals */
    Connect();
}

ProjectView::~ProjectView()
{
    /**
     * Set the source Model as nullpointer so that we don't actually delete
     * the original source model
     */
    proxy->setSourceModel(nullptr);

    proxy->deleteLater();
    delete proxy;
    proxy = nullptr;
}

void ProjectView::Setup()
{
    /* Source Model */
    ProjectModel* model = MBridge::Instance().ProjectDataModel();

    /* Proxy */
    proxy = new ProjectProxyModel(this);
    /* Setup the Proxy's Source Model */
    ResetModel(model);
    setModel(proxy);

    /* Selection Mode */
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformItemSizes(true);
     /* Set Delegate */
    setItemDelegate(new ProjectItemDelegate(this));
    /* Spacing between entries */
    setSpacing(1);

    /* Context Menu */
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void ProjectView::Connect()
{
    connect(this, &QListView::clicked, this, &ProjectView::ItemClicked);
}

void ProjectView::ResetModel(ProjectModel* model)
{
    proxy->setSourceModel(model);
    VOID_LOG_INFO("Project Source Model Updated");
}

void ProjectView::ItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* The source index */
    emit itemClicked(proxy->mapToSource(index));
}

const std::vector<QModelIndex> ProjectView::SelectedIndexes() const
{
    std::vector<QModelIndex> sources;

    /* Get the selection model */
    QItemSelectionModel* selection = selectionModel();

    /* Nothing is selected at the moment */
    if (!selection)
        return sources;

    const QModelIndexList proxyindexes = selection->selectedRows();
    /* We know how many items are selected */
    sources.reserve(proxyindexes.size());

    for (const QModelIndex& index: proxyindexes)
    {
        QModelIndex source = proxy->mapToSource(index);
        if (source.isValid())
            sources.emplace_back(source);
    }

    /* Return the updated source indexes that are selected */
    return sources;
}

bool ProjectView::HasSelection()
{
    /* Underlying selection model */
    QItemSelectionModel* s = selectionModel();

    /* Doesn't have the selection model ?*/
    if (!s)
        return false;

    /* Return whether the selection model has any selection currently */
    return s->hasSelection();
}

void ProjectView::EnableSorting(bool state, const Qt::SortOrder& order)
{
    proxy->sort(state ? 0 : -1, order);
}

VOID_NAMESPACE_CLOSE
