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
#include <QScrollBar>

/* Internal */
#include "PlaylistMediaView.h"
#include "Internal/Descriptors.h"
#include "VoidCore/Logging.h"
#include "VoidObjects/Preferences/Preferences.h"
#include "VoidMediaPlayer/Media/Delegates/ListDelegate.h"
#include "VoidMediaPlayer/Media/Delegates/ThumbnailDelegate.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

PlaylistMediaView::PlaylistMediaView(QWidget* parent)
    : QListView(parent)
    , m_ViewType(ViewType::ListView)
{
    m_PlayAction = new QAction("Play Selected Media");
    m_RemoveAction = new QAction("Remove Selected Media from playlist");

    Setup();
    Connect();
}

PlaylistMediaView::~PlaylistMediaView()
{
    /**
     * Set the source Model as nullpointer so that we don't actually delete
     * the original source model
     */
    m_Proxy->setSourceModel(nullptr);

    m_Proxy->deleteLater();
    delete m_Proxy;
    m_Proxy = nullptr;

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

    // Stacked pixmaps
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
        QPixmap p = indexes.at(i).data(static_cast<int>(EntityModel::MRoles::Thumbnail)).value<QPixmap>();
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
}

void PlaylistMediaView::Setup()
{
    m_Proxy = new EntityProxyModel(this);
    setModel(m_Proxy);

    // Selection Mode
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformItemSizes(true);

    ResetView();

    // Context Menu
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
        setGridSize(QSize());
    }
    else if (m_ViewType == ViewType::DetailedListView)
    {
        setItemDelegate(new MediaItemDelegate(this));
        setViewMode(QListView::ListMode);
        setSpacing(1);
        setResizeMode(QListView::Fixed);
        setGridSize(QSize());
    }
    else
    {
        setItemDelegate(new MediaThumbnailDelegate(this));
        setViewMode(QListView::IconMode);
        setSpacing(2);
        setResizeMode(QListView::Adjust);
        setGridSize(QSize(154, 150)); // Delegate Item::SizeHint().width() + 4, .Height() + 4;
    }
}

void PlaylistMediaView::Connect()
{
    connect(m_PlayAction, &QAction::triggered, this, &PlaylistMediaView::PlaySelected);
    connect(m_RemoveAction, &QAction::triggered, this, &PlaylistMediaView::RemoveSelected);

    connect(this, &QListView::doubleClicked, this, &PlaylistMediaView::ItemDoubleClicked);
    connect(this, &QListView::customContextMenuRequested, this, &PlaylistMediaView::ShowContextMenu);

    // Media Bridge
    connect(&_MediaBridge, &MBridge::projectChanged, this, [this](const Project* project) { ResetModel(nullptr); });
    connect(&_MediaBridge, &MBridge::playlistCreated, this, [this](const Playlist* playlist) { ResetModel(playlist->DataModel()); });
    connect(&_MediaBridge, &MBridge::playlistChanged, this, [this](const Playlist* playlist) { ResetModel(playlist->DataModel()); });
}

void PlaylistMediaView::ResetModel(EntityModel* model)
{
    m_Proxy->setSourceModel(model);
    VOID_LOG_INFO("Playlist Media Source Model Updated");
}

void PlaylistMediaView::ItemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    emit itemDoubleClicked(m_Proxy->mapToSource(index));
}

void PlaylistMediaView::ShowContextMenu(const _QPoint& position)
{
    if (!HasSelection())
        return;

    QMenu contextMenu(this);
    contextMenu.addAction(m_PlayAction);
    contextMenu.addAction(m_RemoveAction);

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
    QItemSelectionModel* selection = selectionModel();
    if (!selection)
        return sources;

    const QModelIndexList proxyindexes = selection->selectedRows();
    sources.reserve(proxyindexes.size());

    for (const QModelIndex& index: proxyindexes)
    {
        QModelIndex source = m_Proxy->mapToSource(index);
        if (source.isValid())
            sources.emplace_back(source);
    }

    return sources;
}

bool PlaylistMediaView::HasSelection()
{
    QItemSelectionModel* s = selectionModel();
    return s && s->hasSelection();
}

void PlaylistMediaView::EnableSorting(bool state, const Qt::SortOrder& order)
{
    m_Proxy->sort(state ? 0 : -1, order);
}

void PlaylistMediaView::SetViewType(const ViewType& type)
{
    m_ViewType = type;
    ResetView();
}

void PlaylistMediaView::PlaySelected()
{
    QItemSelectionModel* selection = selectionModel();
    if (!selection)
        return;

    std::vector<SharedMediaClip> clips;
    const QModelIndexList proxyindexes = selection->selectedRows();
    clips.reserve(proxyindexes.size());

    for (const QModelIndex& index: proxyindexes)
    {
        QModelIndex source = m_Proxy->mapToSource(index);
        if (source.isValid())
            clips.emplace_back(_MediaBridge.PlaylistMediaAt(source));
    }

    emit played(clips);
}

void PlaylistMediaView::RemoveSelected()
{
    QItemSelectionModel* selection = selectionModel();
    if (!selection)
        return;

    const QModelIndexList proxies = selection->selectedRows();
    std::vector<QModelIndex> sources;
    sources.reserve(proxies.size());

    for (const QModelIndex& index : proxies)
    {
        const QModelIndex source = m_Proxy->mapToSource(index);
        if (source.isValid())
            sources.emplace_back(source);
    }

    if (!sources.empty())
    {
        /**
         * sort in reverse as forward iteration would shift the model indexes and
         * result in wrong indexes being deleted, or a worst case scenario result in crashes as
         * the second model index doesn't even exist after the first has been deleted
         */
        std::sort(sources.begin(), sources.end(), [](const QModelIndex& _a, const QModelIndex& _b) -> bool
        {
            return _a.row() > _b.row();
        });
        const int scroll = verticalScrollBar()->value();
        _MediaBridge.RemoveFromPlaylist(sources);

        verticalScrollBar()->setValue(scroll);
    }
}

VOID_NAMESPACE_CLOSE
