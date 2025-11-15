// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PROJECT_MODEL_H
#define _VOID_PROJECT_MODEL_H

/* STD */
#include <vector>

/* Qt */
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Project/Project.h"

VOID_NAMESPACE_OPEN

/**
 * Describes the Project
 */
class VOID_API ProjectModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * Roles for various fields of data from the Project
     */
    enum class Roles
    {
        Name = Qt::UserRole + 1001,
        Active,
        Color,
        SaveState,
    };

public:
    explicit ProjectModel(QObject* parent = nullptr);

    ~ProjectModel();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /* Project */
    void Add(Core::Project* project);
    void Insert(Core::Project* project, int index);
    void Remove(const QModelIndex& index);

    Core::Project* ProjectAt(const QModelIndex& index) const;
    int ProjectRow(const Core::Project* project) const;

    void Clear();

    inline void Refresh() { Update(); }

private: /* Members */
    std::vector<Core::Project*> m_Projects;

private: /* Methods */
    void Update();
};

class VOID_API ProjectProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ProjectProxyModel(QObject* parent = nullptr);

    /* Sets the key which needs to be searched in the data */
    void SetSearchText(const std::string& text);

    /* Sets to role to look at in the model index for data */
    void SetSearchRole(const ProjectModel::Roles& role);

protected:
    /* Returns true for the row that is valid for the search filter */
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

    /* Returns if the left value is lesser than the right value (previous < next )*/
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private: /* Members */
    QString m_SearchText;
    /* The Role to look at while searching */
    int m_SearchRole;

    /* Sorting role */
    int m_SortRole;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_MODEL_H
