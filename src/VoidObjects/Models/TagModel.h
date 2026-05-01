// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TAG_MODEL_H
#define _TAG_MODEL_H

/* STD */
#include <string>
#include <vector>

/* Qt */
#include <QAbstractItemModel>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/Tag.h"

VOID_NAMESPACE_OPEN

class VOID_API TagModel : public QAbstractItemModel
{
    Q_OBJECT

public: /* Enums */
    enum class TagRoles
    {
        Name = Qt::UserRole + 1001,
        Metadata,
        Icon
    };

public:
    explicit TagModel(QObject* parent = nullptr);
    ~TagModel();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool AddTag(const std::string& name);
    bool AddTag(const std::string& name, const TagMetaStruct& metadata);
    bool InsertTag(const std::string& name, int index);
    bool InsertTag(const std::string& name, int index, const TagMetaStruct& metdata);
    void RemoveTag(const QModelIndex& index);
    void RemoveTag(int row);
    Tag* TagAt(const QModelIndex& index) const;
    Tag* TagAt(int row) const;
    void ClearAll();

    inline bool HasTags() const { return !m_Tags.empty(); }
    inline const std::vector<Tag*>& Tags() const { return m_Tags; }

private: /* Members */
    std::vector<Tag*> m_Tags;
};

VOID_NAMESPACE_CLOSE

#endif // _TAG_MODEL_H
