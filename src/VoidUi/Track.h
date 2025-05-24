#ifndef _VOID_TRACK_H
#define _VOID_TRACK_H

/* STD */
#include <vector>
#include <memory>

/* Qt */
#include <QObject>

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
    void SetMedia(const Media& media);

    /*
     * Appends the Media to the already existing track of Medis files
     * Which will get played in order
     */
    void AddMedia(const Media& media);

    /* Clears the Playback Track */
    void Clear();

    /* Getters */
    inline int StartFrame() const { return m_StartFrame; }
    inline int EndFrame() const { return m_EndFrame; }

    inline bool IsEmpty() const { return m_TrackItems.empty(); }

    /*
     * Update the Provided Pointer to VoidImageData with the data of the Media if it exists
     * for the provided frame in the timeline
     * Returns a Bool value to give back a status as to indicate whether the frame exists or not
     */
    bool GetImage(const int frame, VoidImageData* image) const;

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

protected: /* Members */
    
    /* Holds the Media which have been added to the Track */
    std::vector<SharedTrackItem> m_TrackItems;

    int m_StartFrame, m_EndFrame;

protected: /* Methods */
    void SetRange(int start, int end);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_H
