// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Internal */
#include "Sequence.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

PlaybackSequence::PlaybackSequence(QObject* parent)
    : VoidObject(parent)
    , m_StartFrame(0)
    , m_EndFrame(0)
    , m_Recent(nullptr)
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

SharedPlaybackTrack PlaybackSequence::CreateTrack(const Sequence::TrackType& type)
{
    SharedPlaybackTrack track = std::make_shared<PlaybackTrack>(type, this);
    type == Sequence::TrackType::VIDEO ? AddVideoTrack(track) : AddAudioTrack(track);
    return track;
}

SharedPlaybackTrack PlaybackSequence::CreateTrack(const std::string& name, const Sequence::TrackType& type)
{
    SharedPlaybackTrack track = std::make_shared<PlaybackTrack>(type, this);
    track->SetName(name);

    type == Sequence::TrackType::VIDEO ? AddVideoTrack(track) : AddAudioTrack(track);
    return track;
}

SharedPlaybackTrack PlaybackSequence::CreateTrack(const Sequence::TrackType& type, int index)
{
    SharedPlaybackTrack track = std::make_shared<PlaybackTrack>(type, this);
    type == Sequence::TrackType::VIDEO ? AddVideoTrack(track, index) : AddAudioTrack(track, index);
    return track;
}

SharedPlaybackTrack PlaybackSequence::CreateTrack(const std::string& name, const Sequence::TrackType& type, int index)
{
    SharedPlaybackTrack track = std::make_shared<PlaybackTrack>(type, this);
    track->SetName(name);

    type == Sequence::TrackType::VIDEO ? AddVideoTrack(track, index) : AddAudioTrack(track, index);
    return track;
}

void PlaybackSequence::AddVideoTrack(const SharedPlaybackTrack& track)
{
    m_VideoTracks.push_back(track);
    connect(track.get(), &PlaybackTrack::rangeChanged, this, &PlaybackSequence::UpdateRange);
    connect(track.get(), &PlaybackTrack::updated, this, &PlaybackSequence::updated);
    connect(track.get(), &PlaybackTrack::maxEffectsChanged, this, [=]() -> void { emit maxTrackEffectsChanged(track); });

    if (track->Name().empty())
    {
        std::string name;
        name.reserve(15);
        name.append("Video Track ");
        name.append(std::to_string(m_VideoTracks.size()));

        track->SetName(name);
    }

    /**
     * Inorder to update the range on the sequence, we need to see
     * the minimum frame (existing or the provided track) gets set as the start frame
     * and the maximum frame (existing or the provided track) gets set as the end frame
     * 
     * Once the range is set, this will then emit rangeChanged to ensure that it gets notified
     */
    SetRange(std::min(m_StartFrame, track->StartFrame()), std::max(m_EndFrame, track->EndFrame()));
    emit trackAdded(track);
}

void PlaybackSequence::AddAudioTrack(const SharedPlaybackTrack& track)
{
    m_AudioTracks.push_back(track);
    if (track->Name().empty())
    {
        std::string name;
        name.reserve(15);
        name.append("Audio Track ");
        name.append(std::to_string(m_AudioTracks.size()));

        track->SetName(name);
    }

    /**
     * Inorder to update the range on the sequence, we need to see
     * the minimum frame (existing or the provided track) gets set as the start frame
     * and the maximum frame (existing or the provided track) gets set as the end frame
     * 
     * Once the range is set, this will then emit rangeChanged to ensure that it gets notified
     */
    SetRange(std::min(m_StartFrame, track->StartFrame()), std::max(m_EndFrame, track->EndFrame()));
    emit trackAdded(track);
}

void PlaybackSequence::AddVideoTrack(const SharedPlaybackTrack& track, int index)
{
    m_VideoTracks.insert(m_VideoTracks.begin() + index, track);
    connect(track.get(), &PlaybackTrack::rangeChanged, this, &PlaybackSequence::UpdateRange);
    connect(track.get(), &PlaybackTrack::updated, this, &PlaybackSequence::updated);

    if (track->Name().empty())
    {
        std::string name;
        name.reserve(15);
        name.append("Video Track ");
        name.append(std::to_string(m_VideoTracks.size()));

        track->SetName(name);
    }

    /**
     * Inorder to update the range on the sequence, we need to see
     * the minimum frame (existing or the provided track) gets set as the start frame
     * and the maximum frame (existing or the provided track) gets set as the end frame
     * 
     * Once the range is set, this will then emit rangeChanged to ensure that it gets notified
     */
    SetRange(std::min(m_StartFrame, track->StartFrame()), std::max(m_EndFrame, track->EndFrame()));
    emit trackAdded(track);
}

void PlaybackSequence::AddAudioTrack(const SharedPlaybackTrack& track, int index)
{
    m_AudioTracks.insert(m_AudioTracks.begin() + index, track);
    if (track->Name().empty())
    {
        std::string name;
        name.reserve(15);
        name.append("Audio Track ");
        name.append(std::to_string(m_AudioTracks.size()));

        track->SetName(name);
    }

    /**
     * Inorder to update the range on the sequence, we need to see
     * the minimum frame (existing or the provided track) gets set as the start frame
     * and the maximum frame (existing or the provided track) gets set as the end frame
     * 
     * Once the range is set, this will then emit rangeChanged to ensure that it gets notified
     */
    SetRange(std::min(m_StartFrame, track->StartFrame()), std::max(m_EndFrame, track->EndFrame()));
    emit trackAdded(track);
}

void PlaybackSequence::RemoveTrack(const SharedPlaybackTrack& track)
{
    emit trackAboutToBeRemoved(track);
    auto _pred = [track] (const SharedPlaybackTrack& t) -> bool { return track.get() == t.get(); };
    if (track->Type() == Sequence::TrackType::VIDEO)
        m_VideoTracks.erase(std::remove_if(m_VideoTracks.begin(), m_VideoTracks.end(), _pred), m_VideoTracks.end());
    else
        m_AudioTracks.erase(std::remove_if(m_AudioTracks.begin(), m_AudioTracks.end(), _pred), m_AudioTracks.end());
    
    emit trackRemoved();
}

void PlaybackSequence::RemoveTrack(int index, const Sequence::TrackType& type)
{
    if (type == Sequence::TrackType::VIDEO)
    {
        const SharedPlaybackTrack& track = m_VideoTracks[index];
        emit trackAboutToBeRemoved(track);
        m_VideoTracks.erase(m_VideoTracks.begin() + index);
    }
    else
    {
        const SharedPlaybackTrack& track = m_AudioTracks[index];
        emit trackAboutToBeRemoved(track);
        m_AudioTracks.erase(m_AudioTracks.begin() + index);
    }

    emit trackRemoved();
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

int PlaybackSequence::VideoTrackIndex(const PlaybackTrack* track) const
{
    auto _f = [track] (const SharedPlaybackTrack& t) -> bool { return track == t.get(); };
    auto it = std::find_if(m_VideoTracks.begin(), m_VideoTracks.end(), _f);
    return std::distance(m_VideoTracks.begin(), it);
}

int PlaybackSequence::AudioTrackIndex(const PlaybackTrack* track) const
{
    auto _f = [track] (const SharedPlaybackTrack& t) -> bool { return track == t.get(); };
    auto it = std::find_if(m_AudioTracks.begin(), m_AudioTracks.end(), _f);
    return std::distance(m_AudioTracks.begin(), it);
}

bool PlaybackSequence::HasMedia() const
{
    /**
     * A sequence can be said empty if there are no tracks on it
     * But the same sequence can have track(s) but no media in them
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

SharedTrackItem PlaybackSequence::GetTrackItem(const int frame)
{
    // if (m_Recent && m_Recent->InRange(frame))
        // return m_Recent;

    for (auto& track : m_VideoTracks)
    {
        // VOID_LOG_INFO("Looping over: {0} -- Enabled: {1}", track->Name(), track->Enabled());
        if (track->IsEmpty() || !track->Enabled())
            continue;

        if ((m_Recent = track->GetTrackItem(frame)))
            return m_Recent;
    }

    return nullptr;
}

SharedMediaClip PlaybackSequence::Media(v_frame_t frame)
{
    // if (m_Recent && m_Recent->InRange(frame))
        // return m_Recent->GetMedia();

    for (auto& track : m_VideoTracks)
    {
        // VOID_LOG_INFO("Looping over: {0} -- Enabled: {1}", track->Name(), track->Enabled());
        if (track->IsEmpty() || !track->Enabled())
            continue;

        if ((m_Recent = track->GetTrackItem(frame)))
            return m_Recent->GetMedia();
    }

    return nullptr;
}

void PlaybackSequence::Image(v_frame_t frame, FloatImage& image)
{
    // if (m_Recent && m_Recent->InRange(frame))
        // m_Recent->Image(frame, image);

    for (auto& track : m_VideoTracks)
    {
        // VOID_LOG_INFO("Looping over: {0} -- Enabled: {1}", track->Name(), track->Enabled());
        if (track->IsEmpty() || !track->Enabled())
            continue;

        if ((m_Recent = track->GetTrackItem(frame)))
            m_Recent->Image(frame, image);
    }
}

const FloatImage PlaybackSequence::Image(v_frame_t frame)
{
    // if (m_Recent && m_Recent->InRange(frame))
        // return m_Recent->Image(frame);

    for (auto& track : m_VideoTracks)
    {
        // VOID_LOG_INFO("Looping over: {0} -- Enabled: {1}", track->Name(), track->Enabled());
        if (track->IsEmpty() || !track->Enabled())
            continue;

        if ((m_Recent = track->GetTrackItem(frame)))
            return m_Recent->Image(frame);
    }

    return nullptr;
}

void PlaybackSequence::ClearCache()
{
    for (SharedPlaybackTrack& track : m_VideoTracks)
        track->ClearCache();
}

void PlaybackSequence::ClearCache(v_frame_t frame)
{
    /**
     * When the Sequence is asked for an image for a given frame
     * The sequence always returns back the data from the track which is at the top of the stack
     * Meaning bottom of (last added to) the underlying video tracks
     */
    if (!m_VideoTracks.empty() && !m_VideoTracks.back()->IsEmpty()) // TODO: FIX this -- The last Track could be just empty but others above it may not be
        return m_VideoTracks.back()->ClearCache(frame);
}

VOID_NAMESPACE_CLOSE
