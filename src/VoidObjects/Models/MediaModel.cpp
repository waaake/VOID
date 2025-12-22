// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MediaModel.h"
#include "VoidCore/VoidTools.h"

VOID_NAMESPACE_OPEN

/* MediaModel {{{ */

MediaModel::MediaModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex MediaModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid() && row >= 0 && row < static_cast<int>(m_Media.size()))
        return createIndex(row, column, const_cast<SharedMediaClip*>(&m_Media[row]));  // Non-const

    /* Empty */
    return QModelIndex();
}

QModelIndex MediaModel::parent(const QModelIndex& index) const
{
    /* Currently the underlying structure is flat */
    return QModelIndex();
}

int MediaModel::rowCount(const QModelIndex& index) const
{
    /**
     * We have a flat structure as of now,
     * So if the parent is valid (that means it exists in the Model)
     * and there are no indexes which have sub children
     */
    if (index.isValid())
        return 0;

    return static_cast<int>(m_Media.size());
}

int MediaModel::columnCount(const QModelIndex& index) const
{
    return 1;
}

QVariant MediaModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_Media.size()))
        return QVariant();

    const SharedMediaClip& item = m_Media.at(index.row());

    switch (static_cast<MRoles>(role))
    {
        case MRoles::Name:
            return QVariant(item->Name().c_str());
        case MRoles::FrameRange:
            return QVariant(ItemFramerange(item).c_str());
        case MRoles::Extension:
            return QVariant(item->Extension().c_str());
        case MRoles::Thumbnail:
            return item->Thumbnail();
        case MRoles::Framerate:
            return QVariant(ItemFramerate(item).c_str());
        case MRoles::Color:
            return item->Color();
        default:
            return QVariant();
    }
}

Qt::ItemFlags MediaModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

std::string MediaModel::ItemFramerate(const SharedMediaClip& clip) const
{
    std::string framerate = Tools::to_trimmed_string(clip->Framerate());
    framerate += "fps"; // Append

    return framerate;
}

std::string MediaModel::ItemFramerange(const SharedMediaClip& clip) const
{
    std::string range = std::to_string(clip->FirstFrame());
    /* Append */
    range += "-";
    range += std::to_string(clip->LastFrame());

    return  range;
}

void MediaModel::Add(const SharedMediaClip& media)
{
    /* Where the media will be inserted */
    int insertidx = static_cast<int>(m_Media.size());

    beginInsertRows(QModelIndex(), insertidx, insertidx);
    m_Media.push_back(media);
    connect(media.get(), &MediaClip::updated, this, [this, media]() { UpdateMedia(media); });
    endInsertRows();
}

void MediaModel::Insert(const SharedMediaClip& media, const int index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_Media.insert(m_Media.begin() + index, media);
    endInsertRows();
}

void MediaModel::Remove(const QModelIndex& index, bool destroy)
{
    if (!index.isValid())
        return;

    /* Row currently being affected */
    int row = index.row();

    beginRemoveRows(index.parent(), row, row);

    /* Media Clip at the row */
    SharedMediaClip clip = m_Media.at(row);
    m_Media.erase(std::remove(m_Media.begin(), m_Media.end(), clip));

    /* Now Kill the clip */
    if (destroy)
        clip.get()->deleteLater();

    /* End Remove Process */
    endRemoveRows();
}

SharedMediaClip MediaModel::Media(const QModelIndex& index) const
{
    if (!index.isValid())
        return nullptr;

    /* Return the media at the index from the underlying vector */
    return m_Media.at(index.row());
}

int MediaModel::MediaRow(const SharedMediaClip& clip) const
{
    auto it = std::find(m_Media.begin(), m_Media.end(), clip);

    if (it != m_Media.end())
        return static_cast<int>(std::distance(m_Media.begin(), it));

    return -1;
}

SharedMediaClip MediaModel::LastMedia() const
{
    if (m_Media.empty())
        return nullptr;

    return m_Media.back();
}

void MediaModel::Update()
{
    if (m_Media.empty())
        return;

    /* the first item from the struct */
    QModelIndex top = index(0, 0);

    /* last item from the struct */
    QModelIndex bottom = index(static_cast<int>(m_Media.size()) - 1, columnCount() - 1);

    /* Emit that all the data has now been changed */
    emit dataChanged(top, bottom);
}

void MediaModel::UpdateMedia(const SharedMediaClip& clip)
{
    if (m_Media.empty())
        return;

    auto it = std::find(m_Media.begin(), m_Media.end(), clip);
    if (it != m_Media.end())
    {
        QModelIndex idx = index(static_cast<int>(std::distance(m_Media.begin(), it)), 0);
        emit dataChanged(idx, idx);
    }
}

/* }}} */

/* Media Proxy Model {{{ */

MediaProxyModel::MediaProxyModel(QObject * parent)
    : QSortFilterProxyModel(parent)
    , m_SearchText("")
    , m_SearchRole(static_cast<int>(MediaModel::MRoles::Name))
    , m_SortRole(static_cast<int>(MediaModel::MRoles::Name))
{
}

void MediaProxyModel::SetSearchText(const std::string& text)
{
    m_SearchText = text.c_str();

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

void MediaProxyModel::SetSearchRole(const MediaModel::MRoles& role)
{
    m_SearchRole = static_cast<int>(role);

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

bool MediaProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    /* The index from the source model */
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    /* data from the index */
    QString data = sourceIndex.data(m_SearchRole).toString();

    /* Returns true if the data contains the text without any case sensitivity */
    return data.contains(m_SearchText, Qt::CaseInsensitive);
}

bool MediaProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    /* Get the Data (name) from the Source model */
    QString ldata = sourceModel()->index(left.row(), 0, left.parent()).data(m_SortRole).toString();
    QString rdata = sourceModel()->index(right.row(), 0, right.parent()).data(m_SortRole).toString();

    return ldata < rdata;
}

/* }}} */

VOID_NAMESPACE_CLOSE
