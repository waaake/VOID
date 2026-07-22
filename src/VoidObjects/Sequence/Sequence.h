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
#include "VoidObjects/VoidObject.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration of the PlaybackSequence class */
class PlaybackSequence;

typedef std::shared_ptr<PlaybackSequence> SharedPlaybackSequence;

class VOID_API PlaybackSequence : public VoidObject
{
    Q_OBJECT

public:
    PlaybackSequence(QObject* parent = nullptr);

    virtual ~PlaybackSequence();

    /* Clears the Sequence of any tracks that have been added */
    void Clear();
    /* Clears the underlying Tracks' cached data */
    void ClearCache();
    void ClearCache(v_frame_t frame);

    SharedPlaybackTrack CreateTrack(const Sequence::TrackType& type);
    SharedPlaybackTrack CreateTrack(const std::string& name, const Sequence::TrackType& type);
    void AddVideoTrack(const SharedPlaybackTrack& track);
    void AddAudioTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(int index, const Sequence::TrackType& type);

    /* Getters */
    inline int StartFrame() const { return m_StartFrame; }
    inline int EndFrame() const { return m_EndFrame; }

    inline bool IsEmpty() const { return m_VideoTracks.empty() && m_AudioTracks.empty(); }
    inline SharedPlaybackTrack FirstVideoTrack() const { return m_VideoTracks.front(); }

    int NumVideoTracks() const { return static_cast<int>(m_VideoTracks.size()); }
    int NumAudioTracks() const { return static_cast<int>(m_AudioTracks.size()); }

    const SharedPlaybackTrack& VideoTrackAt(std::size_t index) const { return m_VideoTracks.at(index); }
    const SharedPlaybackTrack& AudioTrackAt(std::size_t index) const { return m_AudioTracks.at(index); }

    int VideoTrackIndex(const PlaybackTrack* track) const;
    int AudioTrackIndex(const PlaybackTrack* track) const;

    // const std::vector<SharedPlaybackTrack>& VideoTracks() const { return m_VideoTracks; }
    // const std::vector<SharedPlaybackTrack>& AudioTracks() const { return m_AudioTracks; }

    bool HasMedia() const;

    /* Update the range of the Sequence */
    void SetRange(int start, int end);

    /**
     * Returns the last track that is active
     */
    SharedPlaybackTrack ActiveVideoTrack() const;
    SharedTrackItem GetTrackItem(const int frame);
    SharedMediaClip Media(v_frame_t frame);
    void Image(v_frame_t frame, FloatImage& image);
    const FloatImage Image(v_frame_t frame);

signals: /* Signals denoting actions in the seqeuence */
    void trackAdded(const SharedPlaybackTrack& track);
    void trackAboutToBeRemoved(const SharedPlaybackTrack& track);
    void trackRemoved();
    void cleared();
    void updated();
    void rangeChanged(int start, int end);

protected: /* Members */
    std::vector<SharedPlaybackTrack> m_VideoTracks;
    std::vector<SharedPlaybackTrack> m_AudioTracks;
    SharedTrackItem m_Recent;

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
