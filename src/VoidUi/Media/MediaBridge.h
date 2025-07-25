#ifndef _VOID_MEDIA_GRAPH_H
#define _VOID_MEDIA_GRAPH_H

/* STD */
#include <vector>

/* Qt */
#include <QObject>
#include <QUndoStack>

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/MediaClip.h"
#include "VoidUi/Media/Models/MediaModel.h"

VOID_NAMESPACE_OPEN

/**
 * This class acts as a singleton bridge between various components dealing with Media and MediaClips
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

    /* Disable Copy */
    MBridge(const MBridge&) = delete;

    /* Disable Move */
    MBridge(MBridge&&) = delete;
    MBridge& operator=(MBridge&&) = delete;

    /**
     * Adds Media to the Graph
     */
    bool AddMedia(const MediaStruct& mstruct);
    bool InsertMedia(const MediaStruct& mstruct, const int index);

    /**
     * Removes MediaClip
     * Emits a mediaAboutTobeRemoved signal before removing from the underlying struct
     * to allow components listening to this instance's updates to remove the entity from their structure
     */
    bool Remove(SharedMediaClip clip);
    bool Remove(const QModelIndex& index);

    MediaModel* DataModel() const { return m_Media; }

    /* Push an Undo Command on to the stack */
    void PushCommand(QUndoCommand* command);

    // bool CanRedo() const { return m_UndoStack->canRedo(); }
    // bool CarUndo() const { return m_UndoStack->canUndo(); }

    /* Returns the Menu Actions for Undo and Redo */
    QAction* CreateUndoAction(QObject* parent, const QString& prefix = QString()) const { return m_UndoStack->createUndoAction(parent, prefix); }
    QAction* CreateRedoAction(QObject* parent, const QString& prefix = QString()) const { return m_UndoStack->createRedoAction(parent, prefix); }

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
    MediaModel* m_Media;

    /* Undo Stack */
    QUndoStack* m_UndoStack;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_GRAPH_H
