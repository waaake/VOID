// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ProjectModel.h"
#include "VoidCore/VoidTools.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* Project Model {{{ */

ProjectModel::ProjectModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

ProjectModel::~ProjectModel()
{
    for (int i = m_Projects.size() - 1; i >= 0; --i)
    {
        Core::Project* project = m_Projects.at(i);

        VOID_LOG_INFO("Deleting: {0}", project->Name());
        /* Delete the project */
        project->deleteLater();
        delete project;
        project = nullptr;
    }

    m_Projects.clear();
}

QModelIndex ProjectModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid() && row >= 0 && row < static_cast<int>(m_Projects.size()))
        return createIndex(row, column, const_cast<Core::Project*>(m_Projects[row]));  // Non-const

    /* Empty */
    return QModelIndex();
}

QModelIndex ProjectModel::parent(const QModelIndex& index) const
{
    /* Currently the underlying structure is flat */
    return QModelIndex();
}

int ProjectModel::rowCount(const QModelIndex& index) const
{
    /**
     * We have a flat structure as of now,
     * So if the parent is valid (that means it exists in the Model)
     * and there are no indexes which have sub children
     */
    if (index.isValid())
        return 0;

    return static_cast<int>(m_Projects.size());
}

int ProjectModel::columnCount(const QModelIndex& index) const
{
    return 1;
}

QVariant ProjectModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_Projects.size()))
        return QVariant();

    const Core::Project* project = m_Projects.at(index.row());

    switch (static_cast<Roles>(role))
    {
        case Roles::Name:
            return QVariant(project->Name().c_str());
        case Roles::Active:
            return QVariant(project->Active());
        default:
            return QVariant();
    }

    return QVariant();
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ProjectModel::Add(Core::Project* project)
{
    /* Where the project will be inserted */
    int insertidx = static_cast<int>(m_Projects.size());

    beginInsertRows(QModelIndex(), insertidx, insertidx);
    m_Projects.push_back(project);
    endInsertRows();
}

void ProjectModel::Insert(Core::Project* project, int index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_Projects.insert(m_Projects.begin() + index, project);
    endInsertRows();
}

void ProjectModel::Remove(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* Row currently being affected */
    int row = index.row();

    beginRemoveRows(index.parent(), row, row);

    /* Project at the row */
    Core::Project* project = m_Projects.at(row);

    /* Remove from the vector */
    m_Projects.erase(std::remove(m_Projects.begin(), m_Projects.end(), project));

    /* Delete the Project from the Memory */
    project->deleteLater();
    delete project;
    project = nullptr;

    /* End Remove Process */
    endRemoveRows();
}

Core::Project* ProjectModel::ProjectAt(const QModelIndex& index) const
{
    if (!index.isValid())
        return nullptr;

    /* Return the project at the index from the underlying vector */
    return m_Projects.at(index.row());
}

int ProjectModel::ProjectRow(const Core::Project* project) const
{
    auto it = std::find(m_Projects.begin(), m_Projects.end(), project);

    if (it != m_Projects.end())
        return static_cast<int>(std::distance(m_Projects.begin(), it));

    return -1;
}

void ProjectModel::Update()
{
    if (m_Projects.empty())
        return;

    /* the first item from the struct */
    QModelIndex top = index(0, 0);

    /* last item from the struct */
    QModelIndex bottom = index(static_cast<int>(m_Projects.size()) - 1, columnCount() - 1);

    /* Emit that all the data has now been changed */
    emit dataChanged(top, bottom);
}

/* }}} */

/* Project Proxy Model {{{ */

ProjectProxyModel::ProjectProxyModel(QObject * parent)
    : QSortFilterProxyModel(parent)
    , m_SearchText("")
    , m_SearchRole(static_cast<int>(ProjectModel::Roles::Name))
    , m_SortRole(static_cast<int>(ProjectModel::Roles::Name))
{
}

void ProjectProxyModel::SetSearchText(const std::string& text)
{
    m_SearchText = text.c_str();

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

void ProjectProxyModel::SetSearchRole(const ProjectModel::Roles& role)
{
    m_SearchRole = static_cast<int>(role);

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

bool ProjectProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    /* The index from the source model */
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    /* data from the index */
    QString data = sourceIndex.data(m_SearchRole).toString();

    /* Returns true if the data contains the text without any case sensitivity */
    return data.contains(m_SearchText, Qt::CaseInsensitive);
}

bool ProjectProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    /* Get the Data (name) from the Source model */
    QString ldata = sourceModel()->index(left.row(), 0, left.parent()).data(m_SortRole).toString();
    QString rdata = sourceModel()->index(right.row(), 0, right.parent()).data(m_SortRole).toString();

    return ldata < rdata;
}

/* }}} */

VOID_NAMESPACE_CLOSE
