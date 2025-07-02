#ifndef _VOID_MEDIA_GRAPH_H
#define _VOID_MEDIA_GRAPH_H

/* STD */
#include <vector>

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "MediaClip.h"

VOID_NAMESPACE_OPEN

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

public:
    /* Singleton Instance */
    static MBridge& Instance()
    {
        static MBridge instance;
        return instance;
    }

    /**
     * Adds Media to the Graph
     */
    void AddMedia(const MediaStruct& mstruct);

    /**
     * Removes MediaClip
     * Emits a mediaAboutTobeRemoved signal before removing from the underlying struct
     * to allow components listening to this instance's updates to remove the entity from their structure
     */
    void RemoveClip(SharedMediaClip clip);

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
    std::vector<SharedMediaClip> m_Media;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_GRAPH_H
