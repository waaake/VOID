// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "TrackView.h"

VOID_NAMESPACE_OPEN

TrackView::TrackView(const SharedPlaybackTrack& track, QObject* parent)
    : QObject(parent)
    , m_Track(track)
{
    connect(m_Track.get(), &PlaybackTrack::updated, this, &TrackView::Update);
}

TrackView::~TrackView()
{
    /* If we have a track item -> just remove any references to it */
    if (m_TrackItem)
        m_TrackItem = nullptr;
}

SharedTrackItem TrackView::ItemAt(v_frame_t frame)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    /* The current track item is good for the requested frame */
    if (m_TrackItem && m_TrackItem->HasFrame(frame))
        return m_TrackItem;

    /* Query the item from the track instead and cache that for subsequent queries */
    m_TrackItem = m_Track->GetTrackItem(frame);
    return m_TrackItem;
}

SharedMediaClip TrackView::MediaAt(v_frame_t frame)
{
    /* The current track item is good for the requested frame */
    if (m_TrackItem && m_TrackItem->HasFrame(frame))
        return m_TrackItem->GetMedia();

    /* Query the item from the track instead and cache that for subsequent queries */
    m_TrackItem = m_Track->GetTrackItem(frame);
    return m_TrackItem->GetMedia();
}

void TrackView::Update()
{
    /**
     * For any update done on track needs to allow any subsequenct query to get the item
     * to refresh the underlying item at the given frame
     */
    m_TrackItem = nullptr;
}

void TrackView::Clear()
{
    m_Track = nullptr;
    m_TrackItem = nullptr;
}

VOID_NAMESPACE_CLOSE
