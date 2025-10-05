// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QByteArray>
#include <QDataStream>
#include <QDrag>
#include <QIODevice>
#include <QMimeData>
#include <QPainter>

/* Internal */
#include "MediaView.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Media/Delegates/ListDelegate.h"
#include "VoidUi/Media/Delegates/ThumbnailDelegate.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

MediaView::MediaView(QWidget* parent)
    : QListView(parent)
    , m_ViewType(ViewType::ListView)
{
    Setup();

    /* Connect Signals */
    Connect();
}

MediaView::~MediaView()
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

void MediaView::startDrag(Qt::DropActions supportedActions)
{
    std::vector<QModelIndex> indexes = SelectedIndexes();
    if (indexes.empty())
        return;

    QMimeData* data = new QMimeData();
    data->setData(MimeTypes::MediaItem, _MediaBridge.PackIndexes(indexes));

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

void MediaView::Setup()
{
    /* Set Model */
    /* Source Model */
    MediaModel* model = _MediaBridge.DataModel();

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

void MediaView::ResetView()
{
    if (m_ViewType == ViewType::ListView)
    {
        /* Set Delegate */
        setItemDelegate(new BasicMediaItemDelegate(this));

        /* List View */
        setViewMode(QListView::ListMode);

        /* Spacing between entries */
        setSpacing(1);

        /* Resize Mode */
        setResizeMode(QListView::Fixed);

        /* Reset Grid Size */
        setGridSize(QSize());
    }
    else if (m_ViewType == ViewType::DetailedListView)
    {
        /* Set Delegate */
        setItemDelegate(new MediaItemDelegate(this));

        /* List View */
        setViewMode(QListView::ListMode);

        /* Spacing between entries */
        setSpacing(1);

        /* Resize Mode */
        setResizeMode(QListView::Fixed);

        /* Reset Grid Size */
        setGridSize(QSize());
    }
    else
    {
        /* Set Delegate */
        setItemDelegate(new MediaThumbnailDelegate(this));
        
        /* Icon View */
        setViewMode(QListView::IconMode);
        
        /* Spacing between entries */
        setSpacing(2);
        
        /* Resize Mode */
        setResizeMode(QListView::Adjust);

        /* Grid Size */
        setGridSize(QSize(154, 150)); // Delegate Item::SizeHint().width() + 4, .Height() + 4;
    }
}

void MediaView::Connect()
{
    connect(this, &QListView::doubleClicked, this, &MediaView::ItemDoubleClicked);

    /* Media Bridge */
    connect(&_MediaBridge, &MBridge::projectCreated, this, [this](const Project* project) { ResetModel(project->DataModel()); });
    connect(&_MediaBridge, &MBridge::projectChanged, this, [this](const Project* project) { ResetModel(project->DataModel()); });
}

void MediaView::ResetModel(MediaModel* model)
{
    proxy->setSourceModel(model);
    VOID_LOG_INFO("Source Model Updated");
}

void MediaView::ItemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* The source index */
    emit itemDoubleClicked(proxy->mapToSource(index));
}

const std::vector<QModelIndex> MediaView::SelectedIndexes() const
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

bool MediaView::HasSelection()
{
    /* Underlying selection model */
    QItemSelectionModel* s = selectionModel();

    /* Doesn't have the selection model ?*/
    if (!s)
        return false;

    /* Return whether the selection model has any selection currently */
    return s->hasSelection();
}

void MediaView::EnableSorting(bool state, const Qt::SortOrder& order)
{
    proxy->sort(state ? 0 : -1, order);
}

void MediaView::SetViewType(const ViewType& type)
{
    /* Update the internal view type */
    m_ViewType = type;

    /* Reset the view */
    ResetView();
}

VOID_NAMESPACE_CLOSE
