// Copyright (c) 2025 waaake
// Licensed under the MIT License

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
    m_UndoStack = new QUndoStack(this);
}

MBridge::~MBridge()
{
    m_Media->deleteLater();
    delete m_Media;
    m_Media = nullptr;
}

bool MBridge::AddMedia(const MediaStruct& mstruct)
{
    /* Create the Media Clip */
    SharedMediaClip clip = std::make_shared<MediaClip>(Media(mstruct), this);

    /* Check if the clip is valid, there could be cases we don't have a specific media reader */
    if (clip->Empty())
    {
        VOID_LOG_INFO("Invalid Media.");
        return false;
    }

    /* Add to the underlying struct */
    m_Media->Add(clip);

    /* Emit that we have added a new media clip now */
    emit mediaAdded(clip);

    /* Added successfully */
    return true;
}

bool MBridge::InsertMedia(const MediaStruct& mstruct, const int index)
{
    /* Create the Media Clip */
    SharedMediaClip clip = std::make_shared<MediaClip>(Media(mstruct), this);

    /* Check if the clip is valid, there could be cases we don't have a specific media reader */
    if (clip->Empty())
    {
        VOID_LOG_INFO("Invalid Media.");
        return false;
    }

    /* Add to the underlying struct */
    m_Media->Insert(clip, index);

    /* Emit that we have added a new media clip now */
    emit mediaAdded(clip);

    /* Added successfully */
    return true;

}

bool MBridge::Remove(SharedMediaClip clip)
{
    /* Emit the mediaAboutToBeRemoved signal for all listeners to clear the item */
    emit mediaAboutToBeRemoved(clip);

    /* Ensure All events are Processed before deleting the Media Clip internally */
    QApplication::processEvents();

    /* Create an index from the clip */
    QModelIndex index = m_Media->index(m_Media->MediaRow(clip), 0);

    /* Remove this from the Underlying model */
    m_Media->Remove(index);

    return true;
}

bool MBridge::Remove(const QModelIndex& index)
{
    /* The Media Associated with the Model index */
    SharedMediaClip clip = m_Media->Media(index);

    /* Emit the mediaAboutToBeRemoved signal for all listeners to clear the item */
    emit mediaAboutToBeRemoved(clip);

    /* Ensure All events are Processed before deleting the Media Clip internally */
    QApplication::processEvents();

    /* Remove this from the Underlying model */
    m_Media->Remove(index);

    return true;
}

void MBridge::PushCommand(QUndoCommand* command)
{
    /* Push it on the Undo Stack */
    m_UndoStack->push(command);
}

VOID_NAMESPACE_CLOSE
