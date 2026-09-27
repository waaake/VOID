// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Spreadsheet.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

/// SequenceItemsModel

SequenceItemsModel::SequenceItemsModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_Timekeeper(Timekeeper::Instance())
{
}

QModelIndex SequenceItemsModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() || row < 0 || row > static_cast<int>(m_Items.size())) return QModelIndex();
    return createIndex(row, column, const_cast<SharedTrackItem*>(&m_Items[row]));
}

QModelIndex SequenceItemsModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

int SequenceItemsModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_Items.size());
}

int SequenceItemsModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 11;
}

QVariant SequenceItemsModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.row() < static_cast<int>(m_Items.size()))
    {
        const SharedTrackItem& item = m_Items[index.row()];
        if (role == Qt::DisplayRole)
        {
            switch (index.column())
            {
                case 0  : return index.row();
                case 1  : return QVariant();
                case 2  : return item->Name().c_str();
                case 3  : return QVariant();
                case 4  : return item->Track()->Name().c_str();
                case 5  : return m_Timekeeper.DisplayFrame(item->SourceIn()).c_str();
                case 6  : return m_Timekeeper.DisplayFrame(item->SourceOut()).c_str();
                case 7  : return m_Timekeeper.DisplayFrame(item->TimelineIn()).c_str();
                case 8  : return m_Timekeeper.DisplayFrame(item->TimelineOut()).c_str();
                case 9  : return item->Version().c_str();
                case 10 : return item->NumAvailableVersions();
            }
        }
        else if (role == static_cast<int>(Roles::Color))
        {
            return QVariant(item->Color());
        }
        else if (role == static_cast<int>(Roles::Status))
        {
            return item->Linked();
        }
    }

    return QVariant();
}

Qt::ItemFlags SequenceItemsModel::flags(const QModelIndex& index) const
{
    return index.isValid() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags;
}

QVariant SequenceItemsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
            case 0  : return "Index";
            case 1  : return "Status";
            case 2  : return "Name";
            case 3  : return "Color";
            case 4  : return "Track";
            case 5  : return "Src In";
            case 6  : return "Src Out";
            case 7  : return "Dst In";
            case 8  : return "Dst Out";
            case 9  : return "Version";
            case 10 : return "Num Versions";
        }
    }
    return QVariant();
}

void SequenceItemsModel::ResetSequence(const SharedPlaybackSequence& sequence)
{
    if (m_Sequence)
        disconnect(m_Sequence.get(), &PlaybackSequence::updated, this, nullptr);

    m_Sequence = sequence;
    Reset();
    connect(m_Sequence.get(), &PlaybackSequence::updated, this, &SequenceItemsModel::Reset);
}

SharedTrackItem SequenceItemsModel::Item(const QModelIndex& index) const
{
    if (index.isValid() && index.row() < static_cast<int>(m_Items.size()))
        return m_Items[index.row()];
    return nullptr;
}

void SequenceItemsModel::Reset()
{
    beginResetModel();
    m_Items = m_Sequence->VideoTrackItems();
    endResetModel();
}

/// SequenceItemsProxyModel

SequenceItemsProxyModel::SequenceItemsProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

bool SequenceItemsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    // Not implemented
    return true;
}

bool SequenceItemsProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    int column = sortColumn();
    QVariant ldata = sourceModel()->index(left.row(), column, left.parent()).data();
    QVariant rdata = sourceModel()->index(right.row(), column, right.parent()).data();

    switch (column)
    {
        case 2  :
        case 4  :
        case 5  :
        case 6  :
        case 7  :
        case 8  :
        case 9  :
            return ldata.toString() < rdata.toString();
        case 0  :
        case 10 :
            return ldata.toInt() < rdata.toInt();
        case 1  :
        case 3  :
            return QSortFilterProxyModel::lessThan(left, right);
    }

    return false;
}

VOID_NAMESPACE_CLOSE
