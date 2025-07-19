/* Qt */
#include <QApplication>
#include <QPixmap>

/* Internal */
#include "MediaBridge.h"
#include "VoidCore/VoidTools.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* MediaModel {{{ */

MediaModel::MediaModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex MediaModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid() && row >= 0 && row < static_cast<int>(m_Media.size()))
        return createIndex(row, column, const_cast<SharedMediaClip*>(&m_Media[row]));  // Non-const

    /* Empty */
    return QModelIndex();
}

QModelIndex MediaModel::parent(const QModelIndex& index) const
{
    /* Currently the underlying structure is flat */
    return QModelIndex();
}

int MediaModel::rowCount(const QModelIndex& index) const
{
    /**
     * We have a flat structure as of now,
     * So if the parent is valid (that means it exists in the Model)
     * and there are no indexes which have sub children
     */
    if (index.isValid())
        return 0;

    return static_cast<int>(m_Media.size());
}

int MediaModel::columnCount(const QModelIndex& index) const
{
    return 1;
}

QVariant MediaModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_Media.size()))
        return QVariant();
    
    const SharedMediaClip& item = m_Media.at(index.row());

    switch (static_cast<MRoles>(role))
    {
        case MRoles::Name:
            return QVariant(item->Name().c_str());
        case MRoles::FrameRange:
            return QVariant(ItemFramerange(item).c_str());
        case MRoles::Extension:
            return QVariant(item->Extension().c_str());
        case MRoles::Thumbnail:
            return ItemThumbnail(item);
        case MRoles::Framerate:
            return QVariant(ItemFramerate(item).c_str());
        default:
            return QVariant();
    }
}

Qt::ItemFlags MediaModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

std::string MediaModel::ItemFramerate(const SharedMediaClip& clip) const
{
    std::string framerate = Tools::to_trimmed_string(clip->Framerate());
    framerate += "fps"; // Append

    return framerate;
}

std::string MediaModel::ItemFramerange(const SharedMediaClip& clip) const
{
    std::string range = std::to_string(clip->FirstFrame());
    /* Append */
    range += "-";
    range += std::to_string(clip->LastFrame());

    return  range;
}

QPixmap MediaModel::ItemThumbnail(const SharedMediaClip& clip) const
{   
    /* Grab the pointer to the image data for the first frame to be used as a thumbnail */
    const SharedPixels im = clip->FirstImage();
    QImage::Format format = (im->Channels() == 3) ? QImage::Format_RGB888 : QImage::Format_RGBA8888;

    // /* Resize the Frame to a thumbnail size */
    // QImage scaled = QImage(im->ThumbnailPixels(), im->Width(), im->Height(), format).scaled(
    //         THUMBNAIL_SIZE, THUMBNAIL_SIZE, Qt::KeepAspectRatio
    //     );

    return QPixmap::fromImage(QImage(im->ThumbnailPixels(), im->Width(), im->Height(), format));
}

void MediaModel::Add(const SharedMediaClip& media)
{
    /* Where the media will be inserted */
    int insertidx = static_cast<int>(m_Media.size());

    beginInsertRows(QModelIndex(), insertidx, insertidx);
    m_Media.push_back(media);
    endInsertRows();
}

void MediaModel::Remove(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* Row currently being affected */
    int row = index.row();
    
    beginRemoveRows(index.parent(), row, row);

    /* Media Clip at the row */
    SharedMediaClip clip = m_Media.at(row);

    /* Remove from the vector */
    m_Media.erase(std::remove(m_Media.begin(), m_Media.end(), clip));

    /* Now Kill the clip */
    clip.get()->deleteLater();

    /* End Remove Process */
    endRemoveRows();
}

SharedMediaClip MediaModel::Media(const QModelIndex& index)
{
    if (!index.isValid())
        return nullptr;
    
    /* Return the media at the index from the underlying vector */
    return m_Media.at(index.row());
}

int MediaModel::MediaRow(const SharedMediaClip& clip) const
{
    auto it = std::find(m_Media.begin(), m_Media.end(), clip);

    if (it != m_Media.end())
        return static_cast<int>(std::distance(m_Media.begin(), it));
    
    return -1;
}

void MediaModel::Update()
{
    if (m_Media.empty());
        return;
    
    /* the first item from the struct */
    QModelIndex top = index(0, 0);

    /* last item from the struct */
    QModelIndex bottom = index(static_cast<int>(m_Media.size()) - 1, columnCount() - 1);

    /* Emit that all the data has now been changed */
    emit dataChanged(top, bottom);
}

/* }}} */

/* Media Proxy Model {{{ */

MediaProxyModel::MediaProxyModel(QObject * parent)
    : QSortFilterProxyModel(parent)
    , m_SearchText("")
    , m_SearchRole(MediaModel::MRoles::Name)
{
}

void MediaProxyModel::SetSearchText(const std::string& text)
{
    m_SearchText = text.c_str();

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

void MediaProxyModel::SetSearchRole(const MediaModel::MRoles& role)
{
    m_SearchRole = role;

    /* (Re)filters the underlying model data */
    invalidateFilter();
}

bool MediaProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    /* The index from the source model */
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    /* data from the index */
    QString data = sourceIndex.data(static_cast<int>(m_SearchRole)).toString();

    /* Returns true if the data contains the text without any case sensitivity */
    return data.contains(m_SearchText, Qt::CaseInsensitive);
}

/* }}} */

MBridge::MBridge(QObject* parent)
    : QObject(parent)
{
    m_Media = new MediaModel(this);
}

MBridge::~MBridge()
{
    m_Media->deleteLater();
    delete m_Media;
    m_Media = nullptr;
}

void MBridge::AddMedia(const MediaStruct& mstruct)
{
    /* Create the Media Clip */
    SharedMediaClip clip = std::make_shared<MediaClip>(Media(mstruct), this);

    /* Check if the clip is valid, there could be cases we don't have a specific media reader */
    if (clip->Empty())
    {
        VOID_LOG_INFO("Invalid Media.");
        return;
    }

    /* Add to the underlying struct */
    // m_Media.push_back(clip);
    m_Media->Add(clip);

    /* Emit that we have added a new media clip now */
    emit mediaAdded(clip);
}

void MBridge::Remove(SharedMediaClip clip)
{
    /* Emit the mediaAboutToBeRemoved signal for all listeners to clear the item */
    emit mediaAboutToBeRemoved(clip);

    /* Ensure All events are Processed before deleting the Media Clip internally */
    QApplication::processEvents();

    /* Create an index from the clip */
    QModelIndex index = m_Media->index(m_Media->MediaRow(clip), 0);

    /* Remove this from the Underlying model */
    m_Media->Remove(index);

    // /* Remove from the vector */
    // m_Media.erase(std::remove(m_Media.begin(), m_Media.end(), clip));

    // /* Now Kill the clip */
    // clip.get()->deleteLater();
}

void MBridge::Remove(const QModelIndex& index)
{
    /* The Media Associated with the Model index */
    SharedMediaClip clip = m_Media->Media(index);

    /* Emit the mediaAboutToBeRemoved signal for all listeners to clear the item */
    emit mediaAboutToBeRemoved(clip);

    /* Ensure All events are Processed before deleting the Media Clip internally */
    QApplication::processEvents();

    /* Remove this from the Underlying model */
    m_Media->Remove(index);
}

VOID_NAMESPACE_CLOSE
