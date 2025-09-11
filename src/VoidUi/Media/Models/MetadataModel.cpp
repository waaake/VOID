// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MetadataModel.h"

VOID_NAMESPACE_OPEN

MetadataModel::MetadataModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

void MetadataModel::SetMetadata(const std::map<std::string, std::string>& metadata)
{
    beginResetModel();
    m_Metadata = metadata;
    endResetModel();
}

QModelIndex MetadataModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid() && row >= 0 && row < m_Metadata.size())
        return createIndex(row, column);
    return QModelIndex();
}

QVariant MetadataModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    
    std::map<std::string, std::string>::const_iterator it = m_Metadata.cbegin();
    std::advance(it, index.row());

    return index.column() == 0 ? it->first.c_str() : it->second.c_str();
}

QVariant MetadataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return section == 0 ? "Key" : "Value";

    return QVariant();
}

VOID_NAMESPACE_CLOSE
