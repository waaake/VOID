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
    // if (!parent.isValid() && row >= 0 && row < static_cast<int>(m_Media.size()))
    //     return createIndex(row, column, const_cast<SharedMediaClip*>(&m_Media[row]));  // Non-const
    // At the moment, we don't have nested hierarchy -- it will be there in the future
    if (parent.isValid() || row < 0)
        return QModelIndex();

    if (row < static_cast<int>(m_Media.size()))
        return createIndex(row, column, const_cast<SharedMediaClip*>(&m_Media[row]));

    int srow = row - static_cast<int>(m_Media.size());
    if (srow < static_cast<int>(m_Sequences.size()))
        return createIndex(row, column, const_cast<SharedPlaybackSequence*>(&m_Sequences[srow]));

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

    return static_cast<int>(m_Media.size() + m_Sequences.size());
}

int EntityModel::columnCount(const QModelIndex& index) const
{
    return 1;
}

QVariant EntityModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0)
        return QVariant();

    if (index.row() < static_cast<int>(m_Media.size()))
    {
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
            case MRoles::Type: return static_cast<int>(ProjectEntity::Type::MEDIA);
            default: return QVariant();
        }
    }

    int srow = index.row() - static_cast<int>(m_Media.size());
    if (srow < static_cast<int>(m_Sequences.size()))
    {
        const SharedPlaybackSequence& sequence = m_Sequences.at(srow);
        switch (static_cast<MRoles>(role))
        {
            case MRoles::Name: return QVariant(sequence->Name().c_str());
            case MRoles::FrameRange: return QVariant(ItemFramerange(sequence).c_str());
            case MRoles::Extension: return QVariant();
            case MRoles::Thumbnail: return sequence->Thumbnail();
            case MRoles::Framerate: return QVariant(ItemFramerate(sequence).c_str());
            case MRoles::Color: return sequence->Color();
            case MRoles::Audio: return sequence->HasAudio();
            case MRoles::Tags: return sequence->HasTags();
            case MRoles::Channels: return sequence->Channels();
            case MRoles::Type: return static_cast<int>(ProjectEntity::Type::SEQUENCE);
            default: return QVariant();
        }
    }

    return QVariant();
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

std::string EntityModel::ItemFramerate(const SharedPlaybackSequence& sequence) const
{
    std::string framerate = Tools::to_trimmed_string(sequence->Framerate());
    framerate += "fps";

    return framerate;
}

std::string EntityModel::ItemFramerange(const SharedMediaClip& clip) const
{
    const MFrameRange& r = clip->FrameRange();
    std::string range;
    range.reserve(2 * (r.endframe == 0 ? 1 : static_cast<int>(std::log10(r.endframe)) + 1) + 1);

    range.append(std::to_string(r.startframe));
    range.append("-");
    range.append(std::to_string(r.endframe));

    return  range;
}

std::string EntityModel::ItemFramerange(const SharedPlaybackSequence& sequence) const
{
    const MFrameRange& r = sequence->FrameRange();
    std::string range;
    range.reserve(2 * (r.endframe == 0 ? 1 : static_cast<int>(std::log10(r.endframe)) + 1) + 1);

    range.append(std::to_string(r.startframe));
    range.append("-");
    range.append(std::to_string(r.endframe));

    return  range;
}

void EntityModel::Add(const SharedMediaClip& media)
{
    int insertidx = static_cast<int>(m_Media.size());

    beginInsertRows(QModelIndex(), insertidx, insertidx);
    m_Media.push_back(media);
    connect(media.get(), &MediaClip::updated, this, [this, media]() { UpdateMedia(media); });
    endInsertRows();

    emit updated();
}

void EntityModel::Add(const SharedPlaybackSequence& sequence)
{
    int insertidx = static_cast<int>(m_Media.size() + m_Sequences.size());

    beginInsertRows(QModelIndex(), insertidx, insertidx);
    m_Sequences.push_back(sequence);
    connect(sequence.get(), &PlaybackSequence::updated, this, [this, sequence]() { UpdateSequence(sequence); });
    endInsertRows();

    emit updated();
}

void EntityModel::Insert(const SharedMediaClip& media, const int index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_Media.insert(m_Media.begin() + index, media);
    endInsertRows();

    emit updated();
}

void EntityModel::Insert(const SharedPlaybackSequence& sequence, const int index)
{
    int seqidx = index + static_cast<int>(m_Media.size());
    beginInsertRows(QModelIndex(), seqidx , seqidx);
    m_Sequences.insert(m_Sequences.begin() + index, sequence);
    endInsertRows();

    emit updated();
}

void EntityModel::Remove(const QModelIndex& index, bool destroy)
{
    if (!index.isValid())
        return;

    int row = index.row();
    beginRemoveRows(index.parent(), row, row);

    if (row < static_cast<int>(m_Media.size()))
    {
        SharedMediaClip clip = m_Media.at(row);
        m_Media.erase(std::remove(m_Media.begin(), m_Media.end(), clip));

        /* Now Kill the clip */
        if (destroy)
            clip.get()->deleteLater();
    }

    int srow = row - static_cast<int>(m_Media.size());
    if (srow < static_cast<int>(m_Sequences.size()))
    {
        const SharedPlaybackSequence& sequence = m_Sequences.at(srow);
        m_Sequences.erase(std::remove(m_Sequences.begin(), m_Sequences.end(), sequence));

        if (destroy) sequence.get()->deleteLater();
    }

    endRemoveRows();
    emit updated();
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
    return it == m_Media.end() ? - 1 static_cast<int>(std::distance(m_Media.begin(), it));
}

int EntityModel::SequenceRow(const SharedPlaybackSequence& sequence) const
{
    auto it = std::find(m_Sequences.begin(), m_Sequences.end(), sequence);
    return it == m_Sequences.end() ? - 1 static_cast<int>(std::distance(m_Sequences.begin(), it));
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

// void EntityModel::Update()
// {
//     if (m_Media.empty())
//         return;

//     QModelIndex top = index(0, 0);
//     QModelIndex bottom = index(static_cast<int>(m_Media.size()) - 1, columnCount() - 1);
//     emit dataChanged(top, bottom);
// }

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

void EntityModel::UpdateSequence(const SharedPlaybackSequence& sequence)
{
    if (m_Sequences.empty())
        return;

    auto it = std::find(m_Sequences.begin(), m_Sequences.end(), sequence);
    if (it != m_Sequences.end())
    {
        QModelIndex idx = index(static_cast<int>(m_Media.size()) + static_cast<int>(std::distance(m_Sequences.begin(), it)), 0);
        emit dataChanged(idx, idx);
        emit updated();
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
    invalidateFilter();
}

void EntityProxyModel::SetSearchRole(const EntityModel::MRoles& role)
{
    m_SearchRole = static_cast<int>(role);
    invalidateFilter();
}

bool EntityProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    QString data = sourceIndex.data(m_SearchRole).toString();

    return data.contains(m_SearchText, Qt::CaseInsensitive);
}

bool EntityProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    QString ldata = sourceModel()->index(left.row(), 0, left.parent()).data(m_SortRole).toString();
    QString rdata = sourceModel()->index(right.row(), 0, right.parent()).data(m_SortRole).toString();

    return ldata < rdata;
}

VOID_NAMESPACE_CLOSE
