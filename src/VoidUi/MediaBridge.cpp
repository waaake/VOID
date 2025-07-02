/* Qt */
#include <QApplication>

/* Internal */
#include "MediaBridge.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

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
    m_Media.push_back(clip);

    /* Emit that we have added a new media clip now */
    emit mediaAdded(clip);
}

void MBridge::RemoveClip(SharedMediaClip clip)
{
    /* Emit the mediaAboutToBeRemoved signal for all listeners to clear the item */
    emit mediaAboutToBeRemoved(clip);

    /* Ensure All events are Processed before deleting the Media Clip internally */
    QApplication::processEvents();

    /* Remove from the vector */
    m_Media.erase(std::remove(m_Media.begin(), m_Media.end(), clip));

    /* Now Kill the clip */
    clip.get()->deleteLater();
}

VOID_NAMESPACE_CLOSE
