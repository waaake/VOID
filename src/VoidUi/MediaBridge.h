#ifndef _VOID_MEDIA_GRAPH_H
#define _VOID_MEDIA_GRAPH_H

/* STD */
#include <vector>

/* Qt */
#include <QObject>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

/* Internal */
#include "Definition.h"
#include "MediaClip.h"

VOID_NAMESPACE_OPEN

class MediaModel : public QAbstractItemModel
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
    void Remove(const QModelIndex& index);
    
    SharedMediaClip Media(const QModelIndex& index);
    int MediaRow(const SharedMediaClip& clip) const;

private: /* Members */
    std::vector<SharedMediaClip> m_Media;

private: /* Methods */
    std::string ItemFramerate(const SharedMediaClip& clip) const;
    std::string ItemFramerange(const SharedMediaClip& clip) const;
    QPixmap ItemThumbnail(const SharedMediaClip& clip) const;

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

private: /* Members */
    QString m_SearchText;
    /* The Role to look at while searching */
    MediaModel::MRoles m_SearchRole;
};

/**
 * This class acs as a singleton bridge between various components dealing with Media and MediaClips
 * The class holds all the MediaClips at any instant
 * Any Media which gets added, gets added to this instance
 * this then propagates the added clip to other components by emitting mediaAdded
 * similarly when the media gets removed, a mediaAboutToBeRemoved will be emitted for all components to get
 * rid of the media before the media pointer is finally deleted internally
 */
class MBridge : public QObject
{
    Q_OBJECT

    MBridge(QObject* parent = nullptr);

public:
    /* Singleton Instance */
    static MBridge& Instance()
    {
        static MBridge instance;
        return instance;
    }

    ~MBridge();

    /**
     * Adds Media to the Graph
     */
    void AddMedia(const MediaStruct& mstruct);

    /**
     * Removes MediaClip
     * Emits a mediaAboutTobeRemoved signal before removing from the underlying struct
     * to allow components listening to this instance's updates to remove the entity from their structure
     */
    void Remove(SharedMediaClip clip);
    void Remove(const QModelIndex& index);

    MediaModel* DataModel() const { return m_Media; }

signals:
    /**
     * Media Graph Signals
     *  These will be looked up by other Void Components to see if a media has been added
     *  or is about to be removed to accordingly handle internals
     */
    void updated();
    void mediaAdded(SharedMediaClip);
    void mediaAboutToBeRemoved(SharedMediaClip);

private: /* Members */
    // std::vector<SharedMediaClip> m_Media;
    MediaModel* m_Media;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_GRAPH_H
