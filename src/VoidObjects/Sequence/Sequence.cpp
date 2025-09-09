// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Internal */
#include "Sequence.h"

VOID_NAMESPACE_OPEN

PlaybackSequence::PlaybackSequence(QObject* parent)
    : VoidObject(parent)
    , m_StartFrame(0)
    , m_EndFrame(0)
{
    VOID_LOG_INFO("Sequence Created: {0}", Vuid());
}

PlaybackSequence::~PlaybackSequence()
{
}

void PlaybackSequence::Clear()
{
    /* Clear Video tracks */
    m_AudioTracks.clear();
    m_VideoTracks.clear();

    /* Emit that the timeline was cleared */
    emit cleared();

    /* Reset the Range */
    SetRange(0, 0);
}

void PlaybackSequence::SetRange(int start, int end)
{
    /* Update the internal time range of the sequence */
    m_StartFrame = start;
    m_EndFrame = end;

    /* Emit the rangeChanged signal to notify others */
    emit rangeChanged(m_StartFrame, m_EndFrame);
}

void PlaybackSequence::AddVideoTrack(const SharedPlaybackTrack& track)
{
    /* Update the Video tracks with the provided new track */
    m_VideoTracks.push_back(track);

    /* Connect the signal from the underlying pointer to the Track to updating the range of the sequence */
    connect(track.get(), &PlaybackTrack::rangeChanged, this, &PlaybackSequence::UpdateRange);

    /**
     * Inorder to update the range on the sequence, we need to see
     * the minimum frame (existing or the provided track) gets set as the start frame
     * and the maximum frame (existing or the provided track) gets set as the end frame
     * 
     * Once the range is set, this will then emit rangeChanged to ensure that it gets notified
     */
    SetRange(std::min(m_StartFrame, track->StartFrame()), std::max(m_EndFrame, track->EndFrame()));

    /* Emit that a track has been added */
    emit trackAdded();
}

void PlaybackSequence::AddAudioTrack(const SharedPlaybackTrack& track)
{
    /* Update the Audio tracks with the provided new track */
    m_AudioTracks.push_back(track);

    /**
     * Inorder to update the range on the sequence, we need to see
     * the minimum frame (existing or the provided track) gets set as the start frame
     * and the maximum frame (existing or the provided track) gets set as the end frame
     * 
     * Once the range is set, this will then emit rangeChanged to ensure that it gets notified
     */
    SetRange(std::min(m_StartFrame, track->StartFrame()), std::max(m_EndFrame, track->EndFrame()));

    /* Emit that a track has been added */
    emit trackAdded();
}

void PlaybackSequence::UpdateRange(int start, int end)
{
    /**
     * Inorder to update the range on the sequence, we need to see
     * the minimum frame (existing or the provided start) gets set as the start frame
     * and the maximum frame (existing or the provided end) gets set as the end frame
     * 
     * Once the range is set, this will then emit rangeChanged to ensure that it gets notified
     */
    if (HasMedia())
    {
        /* If we have media on the sequence currently -> keep existing range or increment */
        SetRange(std::min(m_StartFrame, start), std::max(m_EndFrame, end));
    }
    else
    {
        /* The Sequence does not have any media on it, better to set the range which is the least */
        SetRange(start, end);
    }

    VOID_LOG_INFO("Sequence Range Updated. Range: {0}-{1}", m_StartFrame, m_EndFrame);
}

bool PlaybackSequence::HasMedia() const
{
    /**
     * A sequence can be said empty if there are no tracks on it
     * But the same seqeunce can have track(s) but no media in them
     * This means that the sequence is not empty but has no media on it to be played
     */
    if (IsEmpty())    /* Already has no tracks on it */
        return false;

    /* Check if there are any Video tracks that have any media on them */
    for (SharedPlaybackTrack track: m_VideoTracks)
    {
        if (!track->IsEmpty())
            return true;
    }

    /* Check if there are any audio tracks which have any media on them */
    for (SharedPlaybackTrack track: m_AudioTracks)
    {
        if (!track->IsEmpty())
            return true;
    }

    /* The Sequence is empty */
    return false;
}

SharedPlaybackTrack PlaybackSequence::ActiveVideoTrack() const
{
    for (auto it = m_VideoTracks.rbegin(); it != m_VideoTracks.rend(); ++it)
    {
        /**
         * Check if the iterated upon track Active?
         * Meaning its both enabled and visible, if found -> return it
         */
        if ((*it)->Active())
            return *it;
    }

    /* None of the tracks of the sequence are active or there are no tracks at all */
    return nullptr;
}

SharedTrackItem PlaybackSequence::GetTrackItem(const int frame) const
{
    /**
     * When the Sequence is asked for an image for a given frame
     * The sequence always returns back the data from the track which is at the top of the stack
     * Meaning bottom of (last added to) the underlying video tracks
     */
    if (!m_VideoTracks.empty() && !m_VideoTracks.back()->IsEmpty()) // TODO: FIX this -- The last Track could be just empty but others above it may not be
    {
        return m_VideoTracks.back()->GetTrackItem(frame);
    }

    /* There is nothing in the sequence */
    return nullptr;
}

void PlaybackSequence::ClearCache()
{
    for (SharedPlaybackTrack& track : m_VideoTracks)
        track->ClearCache();
}

VOID_NAMESPACE_CLOSE
