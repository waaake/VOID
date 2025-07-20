/* Qt */
#include <QApplication>

/* Internal */
#include "MediaBridge.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

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
