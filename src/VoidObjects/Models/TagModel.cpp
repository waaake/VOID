// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "TagModel.h"

VOID_NAMESPACE_OPEN

TagModel::TagModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

TagModel::~TagModel()
{
    ClearAll();
}

QModelIndex TagModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid() && row >= 0 && row < static_cast<int>(m_Tags.size()))
        return createIndex(row, column, const_cast<Tag*>(m_Tags[row])); // Non-const

    return QModelIndex();
}

QModelIndex TagModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

int TagModel::rowCount(const QModelIndex& parent) const
{        
    return parent.isValid() ? 0 : static_cast<int>(m_Tags.size());
}

int TagModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant TagModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_Tags.size()))
        return QVariant();

    if (role == Qt::DisplayRole)
        return m_Tags[index.row()]->Name().c_str();

    return QVariant();
}

Qt::ItemFlags TagModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void TagModel::AddTag(const std::string& name)
{
    int insertidx = static_cast<int>(m_Tags.size());

    beginInsertRows(QModelIndex(), insertidx, insertidx);
    m_Tags.push_back(new Tag(name));
    endInsertRows();
}

void TagModel::RemoveTag(const QModelIndex& index)
{
    if (!index.isValid() || index.row() > static_cast<int>(m_Tags.size()))
        return;

    const int row = index.row();

    beginRemoveRows(index.parent(), row, row);

    Tag* t = m_Tags[row];
    delete t;
    t = nullptr;

    m_Tags.erase(m_Tags.begin() + row);
    endRemoveRows();
}

void TagModel::RemoveTag(int row)
{
    beginRemoveRows(QModelIndex(), row, row);

    Tag* t = m_Tags[row];
    delete t;
    t = nullptr;

    m_Tags.erase(m_Tags.begin() + row);
    endRemoveRows();
}

Tag* TagModel::TagAt(const QModelIndex& index) const
{
    return index.isValid() && index.row() < static_cast<int>(m_Tags.size()) ? m_Tags[index.row()] : nullptr;
}

Tag* TagModel::TagAt(int row) const
{
    return row < static_cast<int>(m_Tags.size()) ? m_Tags[row] : nullptr;
}

void TagModel::ClearAll()
{
    for (Tag*& tag : m_Tags)
    {
        delete tag;
        tag = nullptr;
    }

    m_Tags.clear();
}

VOID_NAMESPACE_CLOSE
