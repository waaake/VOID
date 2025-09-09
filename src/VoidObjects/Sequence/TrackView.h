// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TRACK_VIEW_H
#define _VOID_TRACK_VIEW_H

/* STD */
#include <mutex>

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "Track.h"

VOID_NAMESPACE_OPEN

/**
 * A track view provides an insight on the underlying items of a track
 * Asking for an item at any frame can be slightly expensive, even with our TrackMap the complexity is O(n log n)
 * so any track item when calculated with that complexity can be cached internally to the track view
 * any subsequent calls to the view to ask for media at a given frame would check internally if the frame is in the
 * range of the current track item, and returns accordingly
 * This improves the overall accessing complexity in terms of time
 */
class VOID_API TrackView : public QObject
{
public:
    TrackView(const SharedPlaybackTrack& track, QObject* parent = nullptr);
    ~TrackView();

    inline SharedPlaybackTrack Track() const { return m_Track; }
    SharedTrackItem ItemAt(v_frame_t frame);
    SharedMediaClip MediaAt(v_frame_t frame);

private: /* Members */
    SharedPlaybackTrack m_Track;
    SharedTrackItem m_TrackItem;
    std::mutex m_Mutex;

private: /* Methods */
    void Clear();
    void Update();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_VIEW_H
