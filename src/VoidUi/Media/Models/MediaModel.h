// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_MODEL_H
#define _VOID_MEDIA_MODEL_H

/* STD */
#include <vector>

/* Qt */
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

/**
 * Describes how the Media is held in a Project/Subdirs
 */
class MediaModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * Roles for various fields of data from the MediaClip
     */
    enum class MRoles
    {
        Name = Qt::UserRole + 1001,
        Framerate,
        Extension,
        FrameRange,
        Thumbnail,
        Color,
    };

public:
    explicit MediaModel(QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /* Media */
    void Add(const SharedMediaClip& media);
    void Insert(const SharedMediaClip& media, const int index);
    void Remove(const QModelIndex& index);
    
    SharedMediaClip Media(const QModelIndex& index);
    int MediaRow(const SharedMediaClip& clip) const;

    void Clear() { m_Media.clear(); }

private: /* Members */
    std::vector<SharedMediaClip> m_Media;

private: /* Methods */
    std::string ItemFramerate(const SharedMediaClip& clip) const;
    std::string ItemFramerange(const SharedMediaClip& clip) const;

    void Update();
};

class MediaProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MediaProxyModel(QObject* parent = nullptr);

    /* Sets the key which needs to be searched in the data */
    void SetSearchText(const std::string& text);

    /* Sets to role to look at in the model index for data */
    void SetSearchRole(const MediaModel::MRoles& role);

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

#endif // _VOID_MEDIA_MODEL_H
