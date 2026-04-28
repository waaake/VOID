// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QByteArray>
#include <QDataStream>
#include <QDrag>
#include <QIODevice>
#include <QMimeData>
#include <QPainter>
#include <QScrollBar>

/* Internal */
#include "MediaView.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

MediaView::MediaView(QWidget* parent)
    : QListView(parent)
    , m_ViewType(ViewType::DetailedListView)
    , m_BasicDelegate(nullptr)
    , m_MediaDelegate(nullptr)
    , m_ThumbnailDelegate(nullptr)
{
    Setup();
    Connect();
}

MediaView::~MediaView()
{
    if (m_BasicDelegate)
    {
        m_BasicDelegate->deleteLater();
        delete m_BasicDelegate;
        m_BasicDelegate = nullptr;
    }

    if (m_MediaDelegate)
    {
        m_MediaDelegate->deleteLater();
        delete m_MediaDelegate;
        m_MediaDelegate = nullptr;
    }

    if (m_ThumbnailDelegate)
    {
        m_ThumbnailDelegate->deleteLater();
        delete m_ThumbnailDelegate;
        m_ThumbnailDelegate = nullptr;
    }

    /**
     * Set the source Model as nullpointer so that we don't actually delete
     * the original source model
     */
    m_Proxy->setSourceModel(nullptr);

    m_Proxy->deleteLater();
    delete m_Proxy;
    m_Proxy = nullptr;
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
    MediaModel* model = _MediaBridge.DataModel();
    m_Proxy = new MediaProxyModel(this);

    /* Setup the Proxy's Source Model */
    ResetModel(model);
    setModel(m_Proxy);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setUniformItemSizes(true);

    ResetView();
}

void MediaView::ResetView()
{
    if (m_ViewType == ViewType::ListView)
    {
        if (!m_BasicDelegate)
        {
            m_BasicDelegate = new BasicMediaItemDelegate(this);
            connect(m_BasicDelegate, &BasicMediaItemDelegate::tagClicked, this, &MediaView::tagClicked);
        }

        setItemDelegate(m_BasicDelegate);

        setViewMode(QListView::ListMode);
        setSpacing(1);
        setResizeMode(QListView::Fixed);
        setGridSize(QSize());
    }
    else if (m_ViewType == ViewType::DetailedListView)
    {
        if (!m_MediaDelegate)
        {
            m_MediaDelegate = new MediaItemDelegate(this);
            connect(m_MediaDelegate, &MediaItemDelegate::tagClicked, this, &MediaView::tagClicked);
        }

        setItemDelegate(m_MediaDelegate);

        setViewMode(QListView::ListMode);
        setSpacing(1);
        setResizeMode(QListView::Fixed);
        setGridSize(QSize());
    }
    else
    {
        if (!m_ThumbnailDelegate)
        {
            m_ThumbnailDelegate = new MediaThumbnailDelegate(this);
            connect(m_ThumbnailDelegate, &MediaThumbnailDelegate::tagClicked, this, &MediaView::tagClicked);
        }

        setItemDelegate(m_ThumbnailDelegate);

        setViewMode(QListView::IconMode);
        setSpacing(2);
        setResizeMode(QListView::Adjust);
        setGridSize(QSize(154, 150)); // Delegate Item::SizeHint().width() + 4, .Height() + 4;
    }

    setDragEnabled(true);
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
    m_Proxy->setSourceModel(model);
    VOID_LOG_INFO("Source Model Updated");
}

void MediaView::ItemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* The source index */
    emit itemDoubleClicked(m_Proxy->mapToSource(index));
}

const std::vector<QModelIndex> MediaView::SelectedIndexes() const
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

bool MediaView::HasSelection()
{
    if (QItemSelectionModel* s = selectionModel())
        return s->hasSelection();

    return false;
}

void MediaView::EnableSorting(bool state, const Qt::SortOrder& order)
{
    m_Proxy->sort(state ? 0 : -1, order);
}

void MediaView::SetViewType(const ViewType& type)
{
    /* Update the internal view type */
    m_ViewType = type;
    ResetView();
}

void MediaView::RemoveSelectedMedia()
{
    if (QItemSelectionModel* selection = selectionModel())
    {
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
            const int scroll = verticalScrollBar()->value();
            _MediaBridge.RemoveMedia(sources);

            verticalScrollBar()->setValue(scroll);
        }
    }
}

VOID_NAMESPACE_CLOSE
