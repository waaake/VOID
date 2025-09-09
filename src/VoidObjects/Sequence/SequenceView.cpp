// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SequenceView.h"

VOID_NAMESPACE_OPEN

SequenceView::SequenceView(const SharedPlaybackSequence& sequence, QObject* parent)
    : QObject(parent)
    , m_Sequence(sequence)
{
    connect(m_Sequence.get(), &PlaybackSequence::updated, this, &SequenceView::Update);
}

SequenceView::~SequenceView()
{
    /* If we have a track item -> just remove any references to it */
    if (m_TrackItem)
        m_TrackItem = nullptr;
}

SharedTrackItem SequenceView::ItemAt(v_frame_t frame)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    /* The current track item is good for the requested frame */
    if (m_TrackItem && m_TrackItem->HasFrame(frame))
        return m_TrackItem;

    /* Query the item from the track instead and cache that for subsequent queries */
    m_TrackItem = m_Sequence->GetTrackItem(frame);
    return m_TrackItem;
}

SharedMediaClip SequenceView::MediaAt(v_frame_t frame)
{
    /* The current track item is good for the requested frame */
    if (m_TrackItem && m_TrackItem->HasFrame(frame))
        return m_TrackItem->GetMedia();

    /* Query the item from the track instead and cache that for subsequent queries */
    m_TrackItem = m_Sequence->GetTrackItem(frame);
    return m_TrackItem->GetMedia();
}

void SequenceView::Update()
{
    /**
     * For any update done on track needs to allow any subsequenct query to get the item
     * to refresh the underlying item at the given frame
     */
    m_TrackItem = nullptr;
}

void SequenceView::Clear()
{
    m_Sequence = nullptr;
    m_TrackItem = nullptr;
}

VOID_NAMESPACE_CLOSE
