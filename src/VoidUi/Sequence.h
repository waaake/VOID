// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_SEQUENCE_H
#define _VOID_SEQUENCE_H

/* STD */
#include <vector>
#include <memory>

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "Track.h"
#include "VoidObject.h"
#include "VoidCore/Media.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration of the PlaybackSequence class */
class PlaybackSequence;

typedef std::shared_ptr<PlaybackSequence> SharedPlaybackSequence;

class PlaybackSequence : public VoidObject
{
    Q_OBJECT

public:
    PlaybackSequence(QObject* parent = nullptr);

    virtual ~PlaybackSequence();

    /* Clears the Sequence of any tracks that have been added */
    void Clear();

    void AddVideoTrack(const SharedPlaybackTrack& track);
    void AddAudioTrack(const SharedPlaybackTrack& track);

    /* Getters */
    inline int StartFrame() const { return m_StartFrame; }
    inline int EndFrame() const { return m_EndFrame; }

    inline bool IsEmpty() const { return m_VideoTracks.empty() && m_AudioTracks.empty(); }
    inline SharedPlaybackTrack FirstVideoTrack() const { return m_VideoTracks.front(); }

    bool HasMedia() const;

    /* Update the range of the Sequence */
    void SetRange(int start, int end);

    /**
     * Returns the last track that is active
     */
    SharedPlaybackTrack ActiveVideoTrack() const;

    SharedTrackItem GetTrackItem(const int frame) const;

signals: /* Signals denoting actions in the seqeuence */
    void trackAdded();
    void cleared();

    /* This signal denotes that something in the sequence was changed/modified i.e. updated */
    void updated();

    /**
     * Emitted when the time range of the sequence has changed
     * includes the start and end frame of the sequence
     */
    void rangeChanged(int start, int end);

protected: /* Members */
    
    /* Holds the tracks that have been added to the seqeuence */
    std::vector<SharedPlaybackTrack> m_VideoTracks;
    std::vector<SharedPlaybackTrack> m_AudioTracks;

    /* Timerange of the sequence */
    int m_StartFrame, m_EndFrame;

private: /* Methods */
    /**
     * This method is responsible for updating the range of the sequence based on
     * any changes that have been made to underlying tracks of whose range has been updated
     * This method checks the current start and end frame and then evaluates the min and max of
     * start and end frames respectively to ensure the range of the seqeunce is not messed up
     */
    void UpdateRange(int start, int end);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_SEQUENCE_H
