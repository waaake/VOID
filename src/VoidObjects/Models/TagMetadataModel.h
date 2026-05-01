// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TAG_METADATA_MODEL_H
#define _TAG_METADATA_MODEL_H

/* STD */
#include <string>
#include <utility>
#include <vector>

/* Qt */
#include <QAbstractTableModel>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

typedef std::vector<std::pair<std::string, std::string>> TagMetaStruct;

class VOID_API TagMetadataModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TagMetadataModel(QObject* parent = nullptr);
    TagMetadataModel(const TagMetaStruct& metadata, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    inline bool IsValid() const { return !m_Metadata.empty(); }
    const TagMetaStruct& Metadata() const { return m_Metadata; }

private:
    TagMetaStruct m_Metadata;
};

VOID_NAMESPACE_CLOSE

#endif // _TAG_METADATA_MODEL_H
