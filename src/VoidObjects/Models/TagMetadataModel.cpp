// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "TagMetadataModel.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

TagMetadataModel::TagMetadataModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int TagMetadataModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_Metadata.size()) + 1;
}

int TagMetadataModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 2;
}

QVariant TagMetadataModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || role != Qt::DisplayRole)
        return QVariant();

    if (index.row() < static_cast<int>(m_Metadata.size()))
        return index.column() == 0 ? m_Metadata[index.row()].first.c_str() : m_Metadata[index.row()].second.c_str();

    return index.column() == 0 ? "<key>" : "<value>";
}

bool TagMetadataModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole)
        return false;

    // Existing
    if (index.row() < static_cast<int>(m_Metadata.size()))
    {
        index.column() == 0
            ? m_Metadata[index.row()].first = value.toString().toStdString()
            : m_Metadata[index.row()].second = value.toString().toStdString();
    }
    else
    {
        index.column() == 0
            ? m_Metadata.push_back({value.toString().toStdString(), ""})
            : m_Metadata.push_back({"", value.toString().toStdString()});
    }
    
    emit dataChanged(index, index);
    emit layoutChanged();
    return true;
}

QVariant TagMetadataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return section == 0 ? "Key" : "Value";

    return QVariant();
}

Qt::ItemFlags TagMetadataModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

VOID_NAMESPACE_CLOSE
