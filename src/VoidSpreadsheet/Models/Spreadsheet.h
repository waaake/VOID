// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <vector>

/* Qt */
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class SequenceItemsModel : public QAbstractItemModel
{
// public:
//     enum class Roles
//     {
//         Event,
//         Name,
//         Track,
//         SrcIn,
//         SrcOut,
//         DstIn,
//         DstOut,
//         Version,
//         NumVersions,
//         Status,
//     };
public:
    explicit SequenceItemsModel(QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void ResetSequence(const SharedPlaybackSequence& sequence);
    SharedTrackItem Item(const QModelIndex& index) const;

private:
    std::vector<SharedTrackItem> m_Items;
    SharedPlaybackSequence m_Sequence;

private:
    void Reset();
};

class SequenceItemsProxyModel : public QSortFilterProxyModel
{
public:
    explicit SequenceItemsProxyModel(QObject* parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
};

VOID_NAMESPACE_CLOSE
