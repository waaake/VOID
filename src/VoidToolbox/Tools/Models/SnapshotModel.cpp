// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SnapshotModel.h"

VOID_NAMESPACE_OPEN

SnapshotModel::SnapshotModel(std::vector<Snapshot>& snapshots, QObject* parent)
    : QAbstractItemModel(parent)
    , m_Snapshots(snapshots)
{
}

QModelIndex SnapshotModel::index(int row, int column, const QModelIndex& parent) const
{
    return QAbstractItemModel::createIndex(row, column);
}

int SnapshotModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_Snapshots.size());
}

QVariant SnapshotModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.row() < static_cast<int>(m_Snapshots.size()))
    {
        const Snapshot& snapshot = m_Snapshots[index.row()];
        if (role == Qt::DisplayRole)
            return index.column() == 0 ? snapshot.name.c_str() : snapshot.description.c_str();
    }

    return QVariant();
}

QVariant SnapshotModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return section == 0 ? "Name" : "Description";

    return QVariant();
}

void SnapshotModel::Remove(const QModelIndex& index)
{
    if (index.isValid() && index.row() < static_cast<int>(m_Snapshots.size()))
    {
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        m_Snapshots.erase(m_Snapshots.begin() + index.row());
        endRemoveRows();
    }
}

void SnapshotModel::Clear()
{
    beginRemoveRows(QModelIndex(), 0, static_cast<int>(m_Snapshots.size()));
    m_Snapshots.clear();
    endRemoveRows();
}

VOID_NAMESPACE_CLOSE
