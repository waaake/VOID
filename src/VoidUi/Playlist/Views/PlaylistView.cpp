// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QDragEnterEvent>
#include <QMimeData>

/* Internal */
#include "PlaylistView.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Playlist/Delegates/ListDelegate.h"

VOID_NAMESPACE_OPEN

PlaylistView::PlaylistView(QWidget* parent)
    : QListView(parent)
{
    Setup();

    /* Connect Signals */
    Connect();
}

PlaylistView::~PlaylistView()
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

void PlaylistView::Setup()
{
    /* Proxy Model */
    proxy = new PlaylistProxyModel(this);

    /* Source Model */
    // ProjectModel* model = MBridge::Instance().ProjectDataModel();
    Project* project = MBridge::Instance().ActiveProject();

    /* Setup the Proxy's Source Model */
    ResetModel(project->PlaylistMediaModel());

    /* Selection Mode */
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformItemSizes(true);
    /* Set Delegate */
    setItemDelegate(new PlaylistItemDelegate(this));
    /* Spacing between entries */
    setSpacing(1);

    /* Context Menu */
    setContextMenuPolicy(Qt::CustomContextMenu);

    setAcceptDrops(true);
    setDropIndicatorShown(true);
}

void PlaylistView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        event->acceptProposedAction();
    }
}

void PlaylistView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        event->acceptProposedAction();
    }
}

void PlaylistView::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        #if _QT6
        QModelIndex index = indexAt(event->position());
        #else
        QModelIndex index = indexAt(event->pos());
        #endif // _QT6

        if (!index.isValid())
            return;

        Playlist* playlist = MBridge::Instance().PlaylistAt(index);
        
        if (!playlist)
            return;

        QByteArray data = event->mimeData()->data(MimeTypes::MediaItem);
        
        /* Read Input data */
        QDataStream stream(&data, QIODevice::ReadOnly);
        int row, column;
        stream >> row >> column;
        
        /**
         * Media from the Media Bridge
         * The media is always retrieved from the active project
         * the assumption is that a drag-drop event would always happen when the project is active
         */
        SharedMediaClip media = MBridge::Instance().Media(row, column);

        playlist->AddMedia(media);
        MBridge::Instance().ActiveProject()->RefreshPlaylist();
    }
}

void PlaylistView::Connect()
{
    connect(this, &QListView::clicked, this, &PlaylistView::ItemClicked);
    connect(&MBridge::Instance(), &MBridge::projectCreated, this, &PlaylistView::ProjectChanged);
    connect(&MBridge::Instance(), &MBridge::projectChanged, this, &PlaylistView::ProjectChanged);
}

void PlaylistView::ResetModel(PlaylistModel* model)
{
    proxy->setSourceModel(model);
    setModel(proxy);
    VOID_LOG_INFO("Playlist Source Model Updated");
}

void PlaylistView::ItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* The source index */
    emit itemClicked(proxy->mapToSource(index));
}

const std::vector<QModelIndex> PlaylistView::SelectedIndexes() const
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

bool PlaylistView::HasSelection()
{
    /* Underlying selection model */
    QItemSelectionModel* s = selectionModel();

    /* Doesn't have the selection model ?*/
    if (!s)
        return false;

    /* Return whether the selection model has any selection currently */
    return s->hasSelection();
}

void PlaylistView::EnableSorting(bool state, const Qt::SortOrder& order)
{
    proxy->sort(state ? 0 : -1, order);
}

void PlaylistView::ProjectChanged(const Project* project)
{
    ResetModel(project->PlaylistMediaModel());   
}

VOID_NAMESPACE_CLOSE
