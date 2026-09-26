// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Spreadsheet.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

/// SequenceItemsModel

SequenceItemsModel::SequenceItemsModel(QObject* parent)
    : QAbstractItemModel(parent)
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
    return parent.isValid() ? 0 : 9;
}

QVariant SequenceItemsModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.row() < static_cast<int>(m_Items.size()) && role == Qt::DisplayRole)
    {
        const SharedTrackItem& item = m_Items[index.row()];
        switch (index.column())
        {
            case 0: return index.row();
            case 1: return item->Name().c_str();
            case 2: return item->Track()->Name().c_str();
            case 3: return static_cast<int>(item->SourceIn());
            case 4: return static_cast<int>(item->SourceOut());
            case 5: return static_cast<int>(item->TimelineIn());
            case 6: return static_cast<int>(item->TimelineOut());
            case 7: return QString("-");
            case 8: return 0;
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
            case 0: return "Index";
            case 1: return "Name";
            case 2: return "Track";
            case 3: return "Src In";
            case 4: return "Src Out";
            case 5: return "Dst In";
            case 6: return "Dst Out";
            case 7: return "Version";
            case 8: return "Num Versions";
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
        case 0:
        case 1:
        case 2:
            return ldata.toString() < rdata.toString();
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            return ldata.toInt() < rdata.toInt();
    }

    return false;
}

VOID_NAMESPACE_CLOSE
