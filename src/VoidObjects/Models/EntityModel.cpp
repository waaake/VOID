// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "EntityModel.h"
#include "VoidCore/VoidTools.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/// EntityModel

EntityModel::EntityModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex EntityModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid() && row >= 0 && row < static_cast<int>(m_Media.size()))
        return createIndex(row, column, const_cast<SharedMediaClip*>(&m_Media[row]));  // Non-const

    return QModelIndex();
}

QModelIndex EntityModel::parent(const QModelIndex& index) const
{
    // Currently the underlying structure is flat
    return QModelIndex();
}

int EntityModel::rowCount(const QModelIndex& index) const
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

int EntityModel::columnCount(const QModelIndex& index) const
{
    return 1;
}

QVariant EntityModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_Media.size()))
        return QVariant();

    const SharedMediaClip& item = m_Media.at(index.row());

    switch (static_cast<MRoles>(role))
    {
        case MRoles::Name: return QVariant(item->Name().c_str());
        case MRoles::FrameRange: return QVariant(ItemFramerange(item).c_str());
        case MRoles::Extension: return QVariant(item->Extension().c_str());
        case MRoles::Thumbnail: return item->Thumbnail();
        case MRoles::Framerate: return QVariant(ItemFramerate(item).c_str());
        case MRoles::Color: return item->Color();
        case MRoles::Audio: return item->HasAudio();
        case MRoles::Tags: return item->HasTags();
        case MRoles::Channels: return item->Channels();
        default: return QVariant();
    }
}

Qt::ItemFlags EntityModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

bool EntityModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
    if (sourceRow < 0 || sourceRow >= static_cast<int>(m_Media.size()) ||
        destinationChild < 0 || destinationChild >= static_cast<int>(m_Media.size()) || count != 1)
        return false;

    /**
     * TODO: The endMoveRows() gives a crash when moving rows where sourceRow and destinationRow have a difference of 1
     * e.g. sourceRow is 2 and destinationRow is 3 or anything similar
     * Need to see why that is happening and a better way around that. maybe I'm not doing this correctly...
     */
    beginMoveRows(sourceParent, sourceRow, sourceRow, destinationParent, destinationChild);
    auto media = m_Media[sourceRow];
    m_Media.erase(m_Media.begin() + sourceRow);

    m_Media.insert(m_Media.begin() + destinationChild, media);
    endMoveRows();
    return true;
}

std::string EntityModel::ItemFramerate(const SharedMediaClip& clip) const
{
    std::string framerate = Tools::to_trimmed_string(clip->Framerate());
    framerate += "fps";

    return framerate;
}

std::string EntityModel::ItemFramerange(const SharedMediaClip& clip) const
{
    std::string range = std::to_string(clip->FirstFrame());
    range += "-";
    range += std::to_string(clip->LastFrame());

    return  range;
}

void EntityModel::Add(const SharedMediaClip& media)
{
    int insertidx = static_cast<int>(m_Media.size());

    beginInsertRows(QModelIndex(), insertidx, insertidx);
    m_Media.push_back(media);
    connect(media.get(), &MediaClip::updated, this, [this, media]() { UpdateMedia(media); });
    endInsertRows();
}

void EntityModel::Insert(const SharedMediaClip& media, const int index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_Media.insert(m_Media.begin() + index, media);
    endInsertRows();
}

void EntityModel::Remove(const QModelIndex& index, bool destroy)
{
    if (!index.isValid())
        return;

    int row = index.row();
    beginRemoveRows(index.parent(), row, row);

    /* Media Clip at the row */
    SharedMediaClip clip = m_Media.at(row);
    m_Media.erase(std::remove(m_Media.begin(), m_Media.end(), clip));

    /* Now Kill the clip */
    if (destroy)
        clip.get()->deleteLater();

    endRemoveRows();
}

SharedMediaClip EntityModel::Media(const QModelIndex& index) const
{
    if (!index.isValid())
        return nullptr;

    return m_Media.at(index.row());
}

int EntityModel::MediaRow(const SharedMediaClip& clip) const
{
    auto it = std::find(m_Media.begin(), m_Media.end(), clip);

    if (it != m_Media.end())
        return static_cast<int>(std::distance(m_Media.begin(), it));

    return -1;
}

SharedMediaClip EntityModel::LastMedia() const
{
    if (m_Media.empty())
        return nullptr;

    return m_Media.back();
}

QModelIndex EntityModel::ShiftIndexUp(const QModelIndex& index)
{
    if (!index.isValid() || index.row() == 0)
        return QModelIndex();

    std::swap(m_Media[index.row()], m_Media[index.row() - 1]);
    return createIndex(index.row() - 1, index.column());
}

QModelIndex EntityModel::ShiftIndexDown(const QModelIndex& index)
{
    if (!index.isValid() || index.row() == m_Media.size() - 1)
        return QModelIndex();

    std::swap(m_Media[index.row()], m_Media[index.row() + 1]);
    return createIndex(index.row() + 1, index.column());
}

void EntityModel::Update()
{
    if (m_Media.empty())
        return;

    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(static_cast<int>(m_Media.size()) - 1, columnCount() - 1);
    emit dataChanged(top, bottom);
}

void EntityModel::UpdateMedia(const SharedMediaClip& clip)
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

/// EntityProxyModel

EntityProxyModel::EntityProxyModel(QObject * parent)
    : QSortFilterProxyModel(parent)
    , m_SearchText("")
    , m_SearchRole(static_cast<int>(EntityModel::MRoles::Name))
    , m_SortRole(static_cast<int>(EntityModel::MRoles::Name))
{
}

void EntityProxyModel::SetSearchText(const std::string& text)
{
    m_SearchText = text.c_str();

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

void EntityProxyModel::SetSearchRole(const EntityModel::MRoles& role)
{
    m_SearchRole = static_cast<int>(role);

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

bool EntityProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    /* The index from the source model */
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    /* data from the index */
    QString data = sourceIndex.data(m_SearchRole).toString();

    /* Returns true if the data contains the text without any case sensitivity */
    return data.contains(m_SearchText, Qt::CaseInsensitive);
}

bool EntityProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    /* Get the Data (name) from the Source model */
    QString ldata = sourceModel()->index(left.row(), 0, left.parent()).data(m_SortRole).toString();
    QString rdata = sourceModel()->index(right.row(), 0, right.parent()).data(m_SortRole).toString();

    return ldata < rdata;
}

/* }}} */

VOID_NAMESPACE_CLOSE
