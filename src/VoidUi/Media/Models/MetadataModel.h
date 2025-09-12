// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_METADATA_MODEL_H
#define _VOID_MEDIA_METADATA_MODEL_H

/* STD */
#include <map>

/* Qt */
#include <QAbstractItemModel>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class MetadataModel : public QAbstractItemModel
{
public:
    explicit MetadataModel(QObject* parent = nullptr);
    void SetMetadata(const std::map<std::string, std::string>& metadata);
    void Clear();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex&) const override { return QModelIndex(); }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override { return parent.isValid() ? 0 : m_Metadata.size(); }
    /* Key: Value for the Metadata */
    int columnCount(const QModelIndex&) const override { return 2; }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private: /* Members */
    std::map<std::string, std::string> m_Metadata;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_METADATA_MODEL_H
