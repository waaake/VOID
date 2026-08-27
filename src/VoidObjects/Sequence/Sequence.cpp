// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Internal */
#include "Sequence.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

PlaybackSequence::PlaybackSequence(Core::Project* project)
    : ProjectEntity(project)
    , m_Name("Sequence")
    , m_Framerate(24.0)
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
    m_StartFrame = start;
    m_EndFrame = end;

    emit rangeChanged(m_StartFrame, m_EndFrame);
    ResizeBuffer(m_EndFrame - m_StartFrame + 1);
}

void PlaybackSequence::SetFramerate(double framerate)
{
    m_Framerate = framerate;
    emit updated();
}

void PlaybackSequence::SetName(const std::string& name)
{
    m_Name = name;
    emit updated();
}

QPixmap PlaybackSequence::Thumbnail()
{
    return m_Recent ? m_Recent->Thumbnail() : DefaultThumbnail();
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
    ConnectVideoTrack(track);

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
    ConnectAudioTrack(track);

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
    ConnectVideoTrack(track);

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
    ConnectAudioTrack(track);

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
    ResizeBuffer(m_EndFrame - m_StartFrame + 1);
}

const SharedPlaybackTrack& PlaybackSequence::TrackAt(std::size_t index, const Sequence::TrackType& type) const
{
    return type == Sequence::TrackType::VIDEO ? m_VideoTracks.at(index) : m_AudioTracks.at(index);
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

int PlaybackSequence::TrackIndex(const PlaybackTrack* track) const
{
    return track->Type() == Sequence::TrackType::VIDEO ? VideoTrackIndex(track) : AudioTrackIndex(track);
}

bool PlaybackSequence::HasMedia() const
{
    /**
     * A sequence can be said empty if there are no tracks on it
     * But the same sequence can have track(s) but no media in them
     * This means that the sequence is not empty but has no media on it to be played
     */
    if (IsEmpty()) return false;

    for (SharedPlaybackTrack track: m_VideoTracks)
    {
        if (!track->IsEmpty())
            return true;
    }

    for (SharedPlaybackTrack track: m_AudioTracks)
    {
        if (!track->IsEmpty())
            return true;
    }

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

    // for (auto& track : m_VideoTracks)
    // {
    //     // VOID_LOG_INFO("Looping over: {0} -- Enabled: {1}", track->Name(), track->Enabled());
    //     if (track->IsEmpty() || !track->Enabled())
    //         continue;

    //     if ((m_Recent = track->GetTrackItem(frame)))
    //         m_Recent->Image(frame, image);
    // }
    std::size_t index = frame - m_StartFrame;
    if (index < m_FrameBuffer.size())
        return m_FrameBuffer[index].Image(image);
}

const FloatImage PlaybackSequence::Image(v_frame_t frame)
{
    // if (m_Recent && m_Recent->InRange(frame))
        // return m_Recent->Image(frame);

    // for (auto& track : m_VideoTracks)
    // {
    //     // VOID_LOG_INFO("Looping over: {0} -- Enabled: {1}", track->Name(), track->Enabled());
    //     if (track->IsEmpty() || !track->Enabled())
    //         continue;

    //     if ((m_Recent = track->GetTrackItem(frame)))
    //         return m_Recent->Image(frame);
    // }

    std::size_t index = frame - m_StartFrame;
    if (index < m_FrameBuffer.size())
        return m_FrameBuffer[index].Image();

    return nullptr;
}

void PlaybackSequence::ClearCache()
{
    for (SharedPlaybackTrack& track : m_VideoTracks)
        track->ClearCache();
}

void PlaybackSequence::ClearCache(v_frame_t frame)
{
    // /**
    //  * When the Sequence is asked for an image for a given frame
    //  * The sequence always returns back the data from the track which is at the top of the stack
    //  * Meaning bottom of (last added to) the underlying video tracks
    //  */
    // if (!m_VideoTracks.empty() && !m_VideoTracks.back()->IsEmpty()) // TODO: FIX this -- The last Track could be just empty but others above it may not be
    //     return m_VideoTracks.back()->ClearCache(frame);

    std::size_t index = frame - m_StartFrame;
    if (index < m_FrameBuffer.size())
        return m_FrameBuffer[index].Clear();
}

void PlaybackSequence::ConnectVideoTrack(const SharedPlaybackTrack& track)
{
    auto* ptr = track.get();
    connect(ptr, &PlaybackTrack::rangeChanged, this, &PlaybackSequence::UpdateRange);
    connect(ptr, &PlaybackTrack::updated, this, &PlaybackSequence::updated);
    connect(ptr, &PlaybackTrack::maxEffectsChanged, this, [=]() -> void { emit maxTrackEffectsChanged(track); });
    connect(ptr, &PlaybackTrack::itemAdded, this, &PlaybackSequence::HandleNewItem);
    connect(ptr, &PlaybackTrack::itemMoved, this, &PlaybackSequence::HandleItemMoved);
    connect(ptr, &PlaybackTrack::itemRangeChanged, this, &PlaybackSequence::HandleItemRangeChanged);
    connect(ptr, &PlaybackTrack::stateChanged, this, [=]() -> void { HandleTrackStateChanged(track); });
    connect(ptr, &PlaybackTrack::itemStateChanged, this, &PlaybackSequence::HandleItemStateChanged);
}

void PlaybackSequence::ConnectAudioTrack(const SharedPlaybackTrack& track)
{
    auto* ptr = track.get();
    connect(ptr, &PlaybackTrack::rangeChanged, this, &PlaybackSequence::UpdateRange);
    connect(ptr, &PlaybackTrack::updated, this, &PlaybackSequence::updated);
    connect(ptr, &PlaybackTrack::stateChanged, this, [=]() -> void { HandleTrackStateChanged(track); });
}

void PlaybackSequence::ResizeBuffer(std::size_t size)
{
    m_FrameBuffer.resize(size);
    VOID_LOG_INFO("Resized to: {}", size);
}

void PlaybackSequence::UpdateBuffer(const MFrameRange& range)
{
    for (v_frame_t i = range.startframe; i <= range.endframe; ++i)
    {
        SharedTrackItem item = GetTrackItem(i);
        if (item && item->Enabled())
            m_FrameBuffer[i - m_StartFrame] = item->InternalFrame(i);
        else
            m_FrameBuffer[i - m_StartFrame] = SequenceFrame();
    }
    VOID_LOG_INFO("Updated Buffer in range: {} - {}", range.startframe, range.endframe);
}

void PlaybackSequence::HandleNewItem(const SharedTrackItem& item)
{
    UpdateBuffer(item->TimelineRange());
}

void PlaybackSequence::HandleItemMoved(const MFrameRange& current, const MFrameRange& previous)
{
    // Item was moved slightly i.e offsetted
    if (current.Overlaps(previous))
    {
        UpdateBuffer({std::min(previous.startframe, current.startframe), std::max(previous.endframe, current.endframe)});
    }
    else
    {
        UpdateBuffer(previous);
        UpdateBuffer(current);
    }
}

void PlaybackSequence::HandleItemRangeChanged(const MFrameRange& current, const MFrameRange& previous)
{
    UpdateBuffer(current.HeadDiff(previous));
    UpdateBuffer(current.TailDiff(previous));
}

void PlaybackSequence::HandleTrackStateChanged(const SharedPlaybackTrack& track)
{
    for (const SharedTrackItem& item : track->Items())
        UpdateBuffer(item->TimelineRange());
}

void PlaybackSequence::HandleItemStateChanged(const SharedTrackItem& item)
{
    UpdateBuffer(item->TimelineRange());
}

VOID_NAMESPACE_CLOSE
