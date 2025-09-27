// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QByteArray>
#include <QDataStream>
#include <QDrag>
#include <QIODevice>
#include <QMenu>
#include <QMimeData>
#include <QDragEnterEvent>

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
    m_PlayAction = new QAction("Play Selected Media");
    m_RemoveAction = new QAction("Remove Selected Media from playlist");
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

    m_PlayAction->deleteLater();
    delete m_PlayAction;
    m_PlayAction = nullptr;

    m_RemoveAction->deleteLater();
    delete m_RemoveAction;
    m_RemoveAction = nullptr;
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

    data->setData(MimeTypes::PlaylistItem, transferData);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(data);
    QPixmap p = index.data(static_cast<int>(MediaModel::MRoles::Thumbnail)).value<QPixmap>();
    drag->setPixmap(p.scaledToWidth(100, Qt::SmoothTransformation));

    drag->exec();
}

void PlaylistMediaView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        event->acceptProposedAction();
    }
}

void PlaylistMediaView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        event->acceptProposedAction();
    }
}

void PlaylistMediaView::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        Playlist* playlist = _MediaBridge.ActivePlaylist();

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
        SharedMediaClip media = _MediaBridge.MediaAt(row, column);

        playlist->AddMedia(media);
    }
}

void PlaylistMediaView::Refresh()
{
    Playlist* playlist = _MediaBridge.ActivePlaylist();
    if (playlist)
        ResetModel(playlist->DataModel());
    else
        ResetModel(nullptr);
    VOID_LOG_INFO("Refreshed");
}

void PlaylistMediaView::Setup()
{
    /* Proxy */
    proxy = new MediaProxyModel(this);
    setModel(proxy);

    /* Selection Mode */
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformItemSizes(true);

    ResetView();

    /* Context Menu */
    setContextMenuPolicy(Qt::CustomContextMenu);

    setDragEnabled(true);
    setAcceptDrops(true);
}

void PlaylistMediaView::ResetView()
{
    /* Set Delegate */
    setItemDelegate(new PlaylistMediaItemDelegate(this));

    /* Spacing */
    setSpacing(1);
    setResizeMode(QListView::Fixed);
}

void PlaylistMediaView::Connect()
{
    connect(m_PlayAction, &QAction::triggered, this, &PlaylistMediaView::PlaySelected);
    connect(m_RemoveAction, &QAction::triggered, this, &PlaylistMediaView::RemoveSelected);

    connect(this, &QListView::doubleClicked, this, &PlaylistMediaView::ItemDoubleClicked);
    connect(this, &QListView::customContextMenuRequested, this, &PlaylistMediaView::ShowContextMenu);

    /* Media Bridge */
    connect(&_MediaBridge, &MBridge::projectChanged, this, [this](const Project* project) { ResetModel(nullptr); });
    connect(&_MediaBridge, &MBridge::playlistCreated, this, [this](const Playlist* playlist) { ResetModel(playlist->DataModel()); });
    connect(&_MediaBridge, &MBridge::playlistChanged, this, [this](const Playlist* playlist) { ResetModel(playlist->DataModel()); });
}

void PlaylistMediaView::ResetModel(MediaModel* model)
{
    proxy->setSourceModel(model);
    VOID_LOG_INFO("Playlist Media Source Model Updated");
}

void PlaylistMediaView::ItemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* The source index */
    emit itemDoubleClicked(proxy->mapToSource(index));
}

void PlaylistMediaView::ShowContextMenu(const Point& position)
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

void PlaylistMediaView::PlaySelected()
{
    std::vector<SharedMediaClip> clips;

    /* Get the selection model */
    QItemSelectionModel* selection = selectionModel();

    /* Nothing is selected at the moment */
    if (!selection)
        return;

    const QModelIndexList proxyindexes = selection->selectedRows();
    /* We know how many items are selected */
    clips.reserve(proxyindexes.size());

    for (const QModelIndex& index: proxyindexes)
    {
        QModelIndex source = proxy->mapToSource(index);
        if (source.isValid())
            clips.emplace_back(_MediaBridge.PlaylistMediaAt(source));
    }

    emit played(clips);
}

void PlaylistMediaView::RemoveSelected()
{
    /* Get the selection model */
    QItemSelectionModel* selection = selectionModel();

    /* Nothing is selected at the moment */
    if (!selection)
        return;

    Playlist* playlist = _MediaBridge.ActivePlaylist();
        
    const QModelIndexList proxyindexes = selection->selectedRows();
    for (int i = proxyindexes.size() - 1; i >=0; --i)
    {
        QModelIndex source = proxy->mapToSource(proxyindexes.at(i));
        if (source.isValid())
            playlist->RemoveMedia(source);
    }
}

VOID_NAMESPACE_CLOSE
