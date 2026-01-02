// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QByteArray>
#include <QDataStream>
#include <QDrag>
#include <QDragEnterEvent>
#include <QIODevice>
#include <QMenu>
#include <QMimeData>
#include <QPainter>

/* Internal */
#include "PlaylistMediaView.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Media/Delegates/ListDelegate.h"
#include "VoidUi/Media/Delegates/ThumbnailDelegate.h"
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
    std::vector<QModelIndex> indexes = SelectedIndexes();
    if (indexes.empty())
        return;

    QMimeData* data = new QMimeData();
    data->setData(MimeTypes::PlaylistItem, _MediaBridge.PackIndexes(indexes));

    QDrag* drag = new QDrag(this);
    drag->setMimeData(data);

    /* Stacked pixmaps */
    const int count = std::min(static_cast<int>(indexes.size()), 4);
    const int thumbsize = 100;
    const int offset = 10;

    QSize canvas(thumbsize + offset * (count - 1), thumbsize + offset * (count - 1));

    QPixmap stack(canvas);
    stack.fill(Qt::transparent);

    QPainter painter(&stack);
    for (int i = 0; i < count; ++i)
    {
        QPoint pos(i * offset, i * offset);
        QPixmap p = indexes.at(i).data(static_cast<int>(MediaModel::MRoles::Thumbnail)).value<QPixmap>();
        painter.drawPixmap(pos, p.scaled(thumbsize, thumbsize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    painter.end();

    drag->setPixmap(stack);
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
        _MediaBridge.AddToPlaylist(data, playlist);
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
    if (m_ViewType == ViewType::ListView)
    {
        setItemDelegate(new BasicMediaItemDelegate(this));
        setViewMode(QListView::ListMode);
        setSpacing(1);
        setResizeMode(QListView::Fixed);

        /* Reset Grid Size */
        setGridSize(QSize());
    }
    else if (m_ViewType == ViewType::DetailedListView)
    {
        setItemDelegate(new MediaItemDelegate(this));
        setViewMode(QListView::ListMode);
        setSpacing(1);
        setResizeMode(QListView::Fixed);

        /* Reset Grid Size */
        setGridSize(QSize());
    }
    else
    {
        setItemDelegate(new MediaThumbnailDelegate(this));
        setViewMode(QListView::IconMode);
        setSpacing(2);
        setResizeMode(QListView::Adjust);

        /* Grid Size */
        setGridSize(QSize(154, 150)); // Delegate Item::SizeHint().width() + 4, .Height() + 4;
    }
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

    const QModelIndexList proxyindexes = selection->selectedRows();

    std::vector<QModelIndex> sources;
    sources.reserve(proxyindexes.size());

    for (int i = proxyindexes.size() - 1; i >=0; --i)
        sources.emplace_back(proxy->mapToSource(proxyindexes.at(i)));

    _MediaBridge.RemoveFromPlaylist(sources);
}

VOID_NAMESPACE_CLOSE
