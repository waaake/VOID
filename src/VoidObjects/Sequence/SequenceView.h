// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_SEQUENCE_VIEW_H
#define _VOID_SEQUENCE_VIEW_H

/* STD */
#include <mutex>

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "Sequence.h"

VOID_NAMESPACE_OPEN

/**
 * A Sequence view provides an insight on the underlying items of a Sequence
 * Asking for an item at any frame can be slightly expensive, even with our TrackMap the complexity is O(n log n)
 * so any track item when calculated with that complexity can be cached internally to the track view
 * any subsequent calls to the view to ask for media at a given frame would check internally if the frame is in the
 * range of the current track item, and returns accordingly
 * This improves the overall accessing complexity in terms of time
 */
class SequenceView : public QObject
{
public:
    SequenceView(const SharedPlaybackSequence& sequence, QObject* parent = nullptr);
    ~SequenceView();

    inline SharedPlaybackSequence Sequence() const { return m_Sequence; }
    SharedTrackItem ItemAt(v_frame_t frame);
    SharedMediaClip MediaAt(v_frame_t frame);

private: /* Members */
    SharedPlaybackSequence m_Sequence;
    SharedTrackItem m_TrackItem;
    std::mutex m_Mutex;

private: /* Methods */
    void Clear();
    void Update();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_SEQUENCE_VIEW_H
