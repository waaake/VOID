// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QByteArray>
#include <QDataStream>
#include <QDrag>
#include <QIODevice>
#include <QMimeData>

/* Internal */
#include "PlaylistMediaView.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Playlist/Delegates/ListDelegate.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

PlaylistMediaView::PlaylistMediaView(QWidget* parent)
    : QListView(parent)
    , m_ViewType(ViewType::ListView)
{
    Setup();

    /* Connect Signals */
    Connect();
}

PlaylistMediaView::~PlaylistMediaView()
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

void PlaylistMediaView::startDrag(Qt::DropActions supportedActions)
{
    QModelIndex index = currentIndex();
    if (!index.isValid())
        return;

    QMimeData* data = new QMimeData();

    QByteArray transferData;
    QDataStream stream(&transferData, QIODevice::WriteOnly);
    stream << index.row() << index.column();

    data->setData(MimeTypes::MediaItem, transferData);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(data);
    QPixmap p = index.data(static_cast<int>(MediaModel::MRoles::Thumbnail)).value<QPixmap>();
    drag->setPixmap(p.scaledToWidth(100, Qt::SmoothTransformation));

    drag->exec();
}

void PlaylistMediaView::Setup()
{
    /* Set Model */
    /* Source Model */
    MediaModel* model = MBridge::Instance().DataModel();

    /* Proxy */
    proxy = new MediaProxyModel(this);
    /* Setup the Proxy's Source Model */
    ResetModel(model);

    setModel(proxy);

    /* Selection Mode */
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformItemSizes(true);

    ResetView();

    /* Context Menu */
    setContextMenuPolicy(Qt::CustomContextMenu);

    setDragEnabled(true);
}

void PlaylistMediaView::ResetView()
{
    /* Set Delegate */
    setItemDelegate(new PlaylistItemDelegate(this));

    /* Spacing */
    setSpacing(1);
    setResizeMode(QListView::Fixed);
}

void PlaylistMediaView::Connect()
{
    connect(this, &QListView::doubleClicked, this, &PlaylistMediaView::ItemDoubleClicked);

    /* Media Bridge */
    connect(&MBridge::Instance(), &MBridge::projectCreated, this, [this](const Project* project) { ResetModel(project->DataModel()); });
    connect(&MBridge::Instance(), &MBridge::projectChanged, this, [this](const Project* project) { ResetModel(project->DataModel()); });
}

void PlaylistMediaView::ResetModel(MediaModel* model)
{
    proxy->setSourceModel(model);
    VOID_LOG_INFO("Source Model Updated");
}

void PlaylistMediaView::ItemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* The source index */
    emit itemDoubleClicked(proxy->mapToSource(index));
}

const std::vector<QModelIndex> PlaylistMediaView::SelectedIndexes() const
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

bool PlaylistMediaView::HasSelection()
{
    /* Underlying selection model */
    QItemSelectionModel* s = selectionModel();

    /* Doesn't have the selection model ?*/
    if (!s)
        return false;

    /* Return whether the selection model has any selection currently */
    return s->hasSelection();
}

void PlaylistMediaView::EnableSorting(bool state, const Qt::SortOrder& order)
{
    proxy->sort(state ? 0 : -1, order);
}

void PlaylistMediaView::SetViewType(const ViewType& type)
{
    /* Update the internal view type */
    m_ViewType = type;

    /* Reset the view */
    ResetView();
}

VOID_NAMESPACE_CLOSE
