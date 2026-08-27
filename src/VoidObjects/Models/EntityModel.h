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
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

/**
 * Describes how the Entities is held in a Project/Subdirs
 */
class VOID_API EntityModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum class MRoles
    {
        Name = Qt::UserRole + 1001,
        Framerate,
        Extension,
        FrameRange,
        Thumbnail,
        Color,
        Audio,
        Tags,
        Channels,
        Type
    };

public:
    explicit EntityModel(QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    Qt::DropActions supportedDropActions() const override { return Qt::MoveAction; }

    bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild) override;

    /* Media */
    void Add(const SharedMediaClip& media);
    void Add(const SharedPlaybackSequence& sequence);
    void Insert(const SharedMediaClip& media, const int index);
    void Insert(const SharedPlaybackSequence& sequence, const int index);
    void Remove(const QModelIndex& index, bool destroy = true);

    SharedMediaClip Media(const QModelIndex& index) const;
    int MediaRow(const SharedMediaClip& clip) const;

    /**
     * Returns the last/latest media clip from the underlying model
     */
    SharedMediaClip LastMedia() const;

    void Clear() { m_Media.clear(); }

    QModelIndex ShiftIndexUp(const QModelIndex& index);
    QModelIndex ShiftIndexDown(const QModelIndex& index);

    /* Iterator */
    inline std::vector<SharedMediaClip>::const_iterator cbegin() const noexcept { return m_Media.cbegin(); }
    inline std::vector<SharedMediaClip>::const_iterator cend() const noexcept { return m_Media.cend(); }

    inline std::vector<SharedMediaClip>::iterator begin() noexcept { return m_Media.begin(); }
    inline std::vector<SharedMediaClip>::iterator end() noexcept { return m_Media.end(); }

    const std::vector<SharedMediaClip> AllMedia() const { return m_Media; }

private: /* Members */
    std::vector<SharedMediaClip> m_Media;
    std::vector<SharedPlaybackSequence> m_Sequences;

private: /* Methods */
    std::string ItemFramerate(const SharedMediaClip& clip) const;
    std::string ItemFramerate(const SharedPlaybackSequence& sequence) const;
    std::string ItemFramerange(const SharedMediaClip& clip) const;
    std::string ItemFramerange(const SharedPlaybackSequence& sequence) const;

    // void Update();
    void UpdateMedia(const SharedMediaClip& clip);
    void UpdateSequence(const SharedPlaybackSequence& sequence);
};

class VOID_API EntityProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit EntityProxyModel(QObject* parent = nullptr);

    // Sets the key which needs to be searched in the data
    void SetSearchText(const std::string& text);
    // Sets to role to look at in the model index for data
    void SetSearchRole(const EntityModel::MRoles& role);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private: /* Members */
    QString m_SearchText;
    int m_SearchRole;
    int m_SortRole;
};


VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_MODEL_H
