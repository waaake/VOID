// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QDragEnterEvent>
#include <QMenu>
#include <QMimeData>

/* Internal */
#include "PlaylistView.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Playlist/Delegates/ListDelegate.h"

VOID_NAMESPACE_OPEN

PlaylistView::PlaylistView(QWidget* parent)
    : QListView(parent)
{
    m_PlayAction = new QAction("Play");
    m_RemoveAction = new QAction("Remove Playlist");

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

    m_PlayAction->deleteLater();
    delete m_PlayAction;
    m_PlayAction = nullptr;

    m_RemoveAction->deleteLater();
    delete m_RemoveAction;
    m_RemoveAction = nullptr;
}

void PlaylistView::Setup()
{
    /* Proxy Model */
    proxy = new PlaylistProxyModel(this);

    /* Source Model */
    Project* project = _MediaBridge.ActiveProject();

    /* Setup the Proxy's Source Model */
    ResetModel(project->PlaylistMediaModel());

    /* Selection Mode */
    setSelectionMode(QAbstractItemView::SingleSelection);
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
        QModelIndex index = indexAt(event->position().toPoint());
        #else
        QModelIndex index = indexAt(event->pos());
        #endif // _QT6

        if (!index.isValid())
            return;

        Playlist* playlist = _MediaBridge.PlaylistAt(index);
        
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
        _MediaBridge.AddToPlaylist(_MediaBridge.DataModel()->index(row, column), playlist);
    }
}

void PlaylistView::Connect()
{
    connect(this, &QListView::clicked, this, &PlaylistView::ItemClicked);
    connect(&_MediaBridge, &MBridge::projectCreated, this, &PlaylistView::ProjectChanged);
    connect(&_MediaBridge, &MBridge::projectChanged, this, &PlaylistView::ProjectChanged);
    connect(this, &QListView::customContextMenuRequested, this, &PlaylistView::ShowContextMenu);

    connect(m_PlayAction, &QAction::triggered, this, &PlaylistView::Play);
    connect(m_RemoveAction, &QAction::triggered, this, &PlaylistView::RemoveSelected);
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

void PlaylistView::ShowContextMenu(const Point& position)
{
    /* Show up only if we have selection */
    if (!HasSelection())
        return;

    /* Create a context menu */
    QMenu contextMenu(this);

    /* Add the Defined actions */
    contextMenu.addAction(m_PlayAction);
    contextMenu.addAction(m_RemoveAction);

    /* Show Menu */
    #if _QT6
    /**
     * Qt6 mapToGlobal returns QPointF while menu.exec expects QPoint
     */
    contextMenu.exec(mapToGlobal(position).toPoint());
    #else
    contextMenu.exec(mapToGlobal(position));
    #endif // _QT6
}

void PlaylistView::Play()
{
    QItemSelectionModel* selection = selectionModel();
    if (!selection)
        return;

    Playlist* playlist = _MediaBridge.PlaylistAt(proxy->mapToSource(selection->currentIndex()));

    if (playlist)
        emit played(playlist);
}

void PlaylistView::RemoveSelected()
{
    QItemSelectionModel* selection = selectionModel();
    if (!selection)
        return;

    _MediaBridge.RemovePlaylist(proxy->mapToSource(selection->currentIndex()));
    emit updated();
}

VOID_NAMESPACE_CLOSE
