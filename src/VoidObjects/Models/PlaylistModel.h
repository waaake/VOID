// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYLIST_MODEL_H
#define _VOID_PLAYLIST_MODEL_H

/* STD */
#include <vector>

/* Qt */
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Playlist/Playlist.h"

VOID_NAMESPACE_OPEN

/**
 * Describes the Playlist
 */
class VOID_API PlaylistModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * Roles for various fields of data from the Playlist
     */
    enum class Roles
    {
        Name = Qt::UserRole + 1001,
        Active,
        Color,
    };

public:
    explicit PlaylistModel(QObject* parent = nullptr);

    ~PlaylistModel();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /* Playlist */
    void Add(Playlist* playlist);
    void Insert(Playlist* media, int index);
    void Remove(const QModelIndex& index);

    Playlist* GetPlaylist(const QModelIndex& index) const;
    int PlaylistRow(const Playlist* playlist) const;

    void Clear();
    inline void Refresh() { Update(); }

    inline const std::vector<Playlist*>::const_iterator cbegin() const noexcept { return m_Playlists.cbegin(); }
    inline const std::vector<Playlist*>::const_iterator cend() const noexcept { return m_Playlists.cend(); }

    inline std::vector<Playlist*>::iterator begin() noexcept { return m_Playlists.begin(); }
    inline std::vector<Playlist*>::iterator end() noexcept { return m_Playlists.end(); }

private: /* Members */
    std::vector<Playlist*> m_Playlists;

private: /* Methods */
    void Update();
};

class VOID_API PlaylistProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit PlaylistProxyModel(QObject* parent = nullptr);

    /* Sets the key which needs to be searched in the data */
    void SetSearchText(const std::string& text);

    /* Sets to role to look at in the model index for data */
    void SetSearchRole(const PlaylistModel::Roles& role);

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

#endif // _VOID_PLAYLIST_MODEL_H
