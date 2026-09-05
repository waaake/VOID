// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SNAPSHOT_MODEL_H
#define _SNAPSHOT_MODEL_H

/* STD */
#include <vector>

/* Qt */
#include <QAbstractListModel>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Core/Snapshot.h"

VOID_NAMESPACE_OPEN

class SnapshotModel : public QAbstractListModel
{
public:
    enum class Roles { Name, Description };
public:
    SnapshotModel(const std::vector<Snapshot>& snapshots, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
private:
    const std::vector<Snapshot>& m_Snapshots;
};

VOID_NAMESPACE_CLOSE

#endif // _SNAPSHOT_MODEL_H
