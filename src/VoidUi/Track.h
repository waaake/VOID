#ifndef _VOID_TRACK_H
#define _VOID_TRACK_H

/* STD */
#include <vector>
#include <memory>

/* Qt */
#include <QObject>
#include <QColor>

/* Internal */
#include "Definition.h"
#include "TrackItem.h"
#include "VoidCore/Media.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration of the Sequence which holds the Track */
class PlaybackSequence;
class PlaybackTrack;

/*
 * Let the Tracks be auto managed by reference count whether they survive
 * in the world or die or even get killed
 */
typedef std::shared_ptr<PlaybackTrack> SharedPlaybackTrack;

class PlaybackTrack : public QObject
{
    Q_OBJECT

public:
    PlaybackTrack(QObject* parent = nullptr);

    virtual ~PlaybackTrack();

    /*
     * Clears anything in the track and sets the provided media as first
     */
    void SetMedia(const SharedMediaClip& media);

    /* Set a color for the Track */
    inline void SetColor(const QColor& color)
    {
        /* Update the color for the track */
        m_Color = color;
        /* Emit a changed signal */
        emit updated();
    }

    /*
     * Appends the Media to the already existing track of Medis files
     * Which will get played in order
     */
    void AddMedia(const SharedMediaClip& media);

    /* Clears the Playback Track */
    void Clear();

    /*
     * Caches all frames of any media on the track
     * emits frameCached for every frame that has been cached
     */
    void Cache();

    /* Clears all cache from internal media and emits cacheCleared */
    void ClearCache();

    /* Getters */
    inline int StartFrame() const { return m_StartFrame; }
    inline int EndFrame() const { return m_EndFrame; }

    inline bool IsEmpty() const { return m_TrackItems.empty(); }

    /* Returns the Color associated with the Track */
    inline QColor Color() const { return m_Color; }

    /*
     * Update the Provided Pointer to VoidImageData with the data of the Media if it exists
     * for the provided frame in the timeline
     * Returns a Bool value to give back a status as to indicate whether the frame exists or not
     */
    /*
     * This function is marked non-const as we're emitting a signal from the function itself
     * TODO: investigate a better way to handle the signal of frameCached
     */
    bool GetImage(const int frame, VoidImageData* image);

    /*
     * From the track, return the track item which is present at a given frame in the timeline
     * Returns nullptr if there is no trackitem at the given timeframe
     */
    SharedTrackItem GetTrackItem(const int frame) const;

    /* The parent of the Track should always be a Sequence, in case it exists inside a Sequence */
    inline PlaybackSequence* Sequence() const { return reinterpret_cast<PlaybackSequence*>(parent()); }

    /* Setters */

    /*
     * The track's range is always defined by the track items in it
     * The only thing which can/should be changed of a track is the starting frame
     */
    inline void SetStartFrame(int start) { SetRange(start, start + m_EndFrame); }

signals: /* Signals Denoting actions in the Track */
    void mediaAdded();
    void cleared();

    /* This signal denotes that something in the track was changed/modified i.e. updated */
    void updated();

    /*
     * Emitted when the time range of the track has changed
     * includes the start and end frame of the track
     */
    void rangeChanged(int start, int end);

    /*
     * Emitted when a frame is cached
     * The cache could happen when the media cache operation is run continuously on a thread
     * Or if the frame is queried by the viewport
     */
    void frameCached(int frame);

    /* Emitted when the cache of all items' media has been cleared internally */
    void cacheCleared();
protected: /* Members */
    /* Holds the Media which have been added to the Track */
    std::vector<SharedTrackItem> m_TrackItems;

    int m_StartFrame, m_EndFrame;

    /* The color associated with the track */
    QColor m_Color;

protected: /* Methods */
    void SetRange(int start, int end);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_H
