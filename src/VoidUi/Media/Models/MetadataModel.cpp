// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MetadataModel.h"

VOID_NAMESPACE_OPEN

MetadataModel::MetadataModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

void MetadataModel::SetMetadata(const SharedMediaClip& media)
{
    beginResetModel();

    // Basic metadata from the media
    m_Metadata = media->Metadata();

    // Custom Tags metadata
    for (const Tag* tag : media->Tags())
    {
        for (const auto& [key, value] : tag->Metdata())
        {
            // Key
            std::string key_;
            key_.reserve(6 + tag->Name().size() + 1 + key.size());
            key_.append("Tags/").append(tag->Name()).append("/").append(key);

            m_Metadata[key_] = value;
        }
    }

    endResetModel();
}

void MetadataModel::SetMetadata(const std::map<std::string, std::string>& metadata)
{
    beginResetModel();
    m_Metadata = metadata;
    endResetModel();
}

void MetadataModel::Clear()
{
    beginResetModel();
    m_Metadata.clear();
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
