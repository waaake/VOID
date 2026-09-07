// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SNAPSHOT_MODEL_H
#define _SNAPSHOT_MODEL_H

/* STD */
#include <vector>

/* Qt */
#include <QAbstractItemModel>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Core/Snapshot.h"

VOID_NAMESPACE_OPEN

class SnapshotModel : public QAbstractItemModel
{
public:
    enum class Roles { Name, Description };
public:
    SnapshotModel(std::vector<Snapshot>& snapshots, QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex&) const override { return QModelIndex(); }
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex&) const override { return 2; }
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void Remove(const QModelIndex& index);
    void Clear();

private:
    std::vector<Snapshot>& m_Snapshots;
};

VOID_NAMESPACE_CLOSE

#endif // _SNAPSHOT_MODEL_H
