// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SnapshotModel.h"

VOID_NAMESPACE_OPEN

SnapshotModel::SnapshotModel(const std::vector<Snapshot>& snapshots, QObject* parent)
    : QAbstractListModel(parent)
    , m_Snapshots(snapshots)
{
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
            return snapshot.name.c_str();

        switch (static_cast<SnapshotModel::Roles>(role))
        {
            case SnapshotModel::Roles::Name : return snapshot.name.c_str();
            case SnapshotModel::Roles::Description : return snapshot.description.c_str();
        }
    }

    return QVariant();
}

VOID_NAMESPACE_CLOSE
