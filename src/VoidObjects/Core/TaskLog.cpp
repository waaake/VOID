// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "TaskLog.h"

VOID_NAMESPACE_OPEN

TaskLogModel::TaskLogModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex TaskLogModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() || row >= static_cast<int>(m_Logs.size()))
        return QModelIndex();

    return createIndex(row, column, const_cast<TaskLog*>(&m_Logs[row])); // Non const
}

QModelIndex TaskLogModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

int TaskLogModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_Logs.size());
}

int TaskLogModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant TaskLogModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.row() < static_cast<int>(m_Logs.size()))
    {
        if (role == Qt::DisplayRole)
            return m_Logs[index.row()].LogText();
        if (role == static_cast<int>(TaskLogModel::Roles::LevelRole))
            return static_cast<int>(m_Logs[index.row()].level);
    }

    return QVariant();
}

Qt::ItemFlags TaskLogModel::flags(const QModelIndex& index) const
{
    return index.isValid() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags;
}

void TaskLogModel::AddLog(const TaskLog& log)
{
    int index = static_cast<int>(m_Logs.size());

    beginInsertRows(QModelIndex(), index, index);
    m_Logs.push_back(log);
    endInsertRows();
}

void TaskLogModel::AddLog(const QString& text, const TaskLog::Level& level)
{
    int index = static_cast<int>(m_Logs.size());

    beginInsertRows(QModelIndex(), index, index);
    m_Logs.emplace_back(text, level);
    endInsertRows();
}

void TaskLogModel::Clear()
{
    beginRemoveRows(QModelIndex(), 0, static_cast<int>(m_Logs.size()) - 1);
    m_Logs.clear();
    endRemoveRows(); 
}

/// TaskLogProxyModel

TaskLogProxyModel::TaskLogProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_Level(0)
{
}

void TaskLogProxyModel::SetLogLevel(const TaskLog::Level& level)
{
    m_Level = static_cast<int>(level);
    invalidateFilter();
}

void TaskLogProxyModel::SetLogLevel(int level)
{
    m_Level = level;
    invalidateFilter();
}

bool TaskLogProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    int level = sourceIndex.data(static_cast<int>(TaskLogModel::Roles::LevelRole)).toInt();

    return level >= m_Level;
}

VOID_NAMESPACE_CLOSE
