// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "PlaylistModel.h"
#include "VoidCore/VoidTools.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* Project Model {{{ */

PlaylistModel::PlaylistModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

PlaylistModel::~PlaylistModel()
{
    for (int i = m_Playlists.size() - 1; i >= 0; --i)
    {
        Playlist* playlist = m_Playlists.at(i);

        VOID_LOG_INFO("Deleting Playlist: {0}", playlist->Name());
        /* Delete the playlist */
        playlist->deleteLater();
        delete playlist;
        playlist = nullptr;
    }

    m_Playlists.clear();
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid() && row >= 0 && row < static_cast<int>(m_Playlists.size()))
        return createIndex(row, column, const_cast<Playlist*>(m_Playlists[row]));  // Non-const

    /* Empty */
    return QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex& index) const
{
    /* Currently the underlying structure is flat */
    return QModelIndex();
}

int PlaylistModel::rowCount(const QModelIndex& index) const
{
    /**
     * We have a flat structure as of now,
     * So if the parent is valid (that means it exists in the Model)
     * and there are no indexes which have sub children
     */
    if (index.isValid())
        return 0;

    return static_cast<int>(m_Playlists.size());
}

int PlaylistModel::columnCount(const QModelIndex& index) const
{
    return 1;
}

QVariant PlaylistModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_Playlists.size()))
        return QVariant();

    const Playlist* project = m_Playlists.at(index.row());

    switch (static_cast<Roles>(role))
    {
        case Roles::Name:
            return QVariant(project->Name().c_str());
        default:
            return QVariant();
    }

    return QVariant();
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void PlaylistModel::Add(Playlist* playlist)
{
    /* Where the playlist will be inserted */
    int insertidx = static_cast<int>(m_Playlists.size());

    beginInsertRows(QModelIndex(), insertidx, insertidx);
    m_Playlists.push_back(playlist);
    endInsertRows();
}

void PlaylistModel::Insert(Playlist* playlist, int index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_Playlists.insert(m_Playlists.begin() + index, playlist);
    endInsertRows();
}

void PlaylistModel::Remove(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* Row currently being affected */
    int row = index.row();

    beginRemoveRows(index.parent(), row, row);

    /* Playlist at the row */
    Playlist* playlist = m_Playlists.at(row);

    /* Remove from the vector */
    m_Playlists.erase(std::remove(m_Playlists.begin(), m_Playlists.end(), playlist));

    /* Delete the Playlist from the Memory */
    playlist->deleteLater();
    delete playlist;
    playlist = nullptr;

    /* End Remove Process */
    endRemoveRows();
}

Playlist* PlaylistModel::GetPlaylist(const QModelIndex& index) const
{
    if (!index.isValid())
        return nullptr;

    /* Return the playlist at the index from the underlying vector */
    return m_Playlists.at(index.row());
}

int PlaylistModel::PlaylistRow(const Playlist* playlist) const
{
    auto it = std::find(m_Playlists.begin(), m_Playlists.end(), playlist);

    if (it != m_Playlists.end())
        return static_cast<int>(std::distance(m_Playlists.begin(), it));

    return -1;
}

void PlaylistModel::Update()
{
    if (m_Playlists.empty())
        return;

    /* the first item from the struct */
    QModelIndex top = index(0, 0);

    /* last item from the struct */
    QModelIndex bottom = index(static_cast<int>(m_Playlists.size()) - 1, columnCount() - 1);

    /* Emit that all the data has now been changed */
    emit dataChanged(top, bottom);
}

/* }}} */

/* Playlist Proxy Model {{{ */

PlaylistProxyModel::PlaylistProxyModel(QObject * parent)
    : QSortFilterProxyModel(parent)
    , m_SearchText("")
    , m_SearchRole(static_cast<int>(PlaylistModel::Roles::Name))
    , m_SortRole(static_cast<int>(PlaylistModel::Roles::Name))
{
}

void PlaylistProxyModel::SetSearchText(const std::string& text)
{
    m_SearchText = text.c_str();

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

void PlaylistProxyModel::SetSearchRole(const PlaylistModel::Roles& role)
{
    m_SearchRole = static_cast<int>(role);

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

bool PlaylistProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    /* The index from the source model */
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    /* data from the index */
    QString data = sourceIndex.data(m_SearchRole).toString();

    /* Returns true if the data contains the text without any case sensitivity */
    return data.contains(m_SearchText, Qt::CaseInsensitive);
}

bool PlaylistProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    /* Get the Data (name) from the Source model */
    QString ldata = sourceModel()->index(left.row(), 0, left.parent()).data(m_SortRole).toString();
    QString rdata = sourceModel()->index(right.row(), 0, right.parent()).data(m_SortRole).toString();

    return ldata < rdata;
}

/* }}} */

VOID_NAMESPACE_CLOSE
