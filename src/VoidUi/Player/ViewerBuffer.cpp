// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ViewerBuffer.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Player/Player.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

static FloatImage s_NullBuffer = nullptr;
static const std::size_t s_MaxBuffers = 1000;

// std::ostream& operator<<(std::ostream& stream, const std::unordered_set<v_frame_t>& s)
// {
//     stream << "unordered_set<v_frame_t> [";
//     for (v_frame_t frame : s)
//         stream << frame << ", ";
//     stream << "]";

//     return stream;
// }

ViewerBuffer::ViewerBuffer(QObject* parent)
    : QObject(parent)
    , m_Name("Viewer")
    , m_Color(130, 110, 190)    // Purple
    , m_Player(nullptr)
    , m_Clip(std::make_shared<MediaClip>())
    , m_Track(std::make_shared<PlaybackTrack>())
    , m_Sequence(std::make_shared<PlaybackSequence>())
    , m_Playlist(nullptr)
    , m_MaxMemory(VoidPreferences::Instance().GetCacheMemory() * 1024 * 1024 * 1024) // 1 GB by default
    , m_Framesize(1)
    , m_Capacity(10)
    , m_Startframe(0)
    , m_Endframe(1)
    , m_LastCached(0)
    , m_PlayingComponent(PlayableComponent::Clip)
    , m_State(PlayState::Forwards)
    , m_BackBuffer(3)
    , m_Active(false)
{
    m_ThreadPool.setMaxThreadCount(VoidPreferences::Instance().GetCacheThreads());
    connect(&m_CacheTimer, &QTimer::timeout, this, &ViewerBuffer::Update, Qt::DirectConnection);
    connect(&VoidPreferences::Instance(), &VoidPreferences::updated, this, &ViewerBuffer::SettingsUpdated);
}

ViewerBuffer::~ViewerBuffer()
{
}

void ViewerBuffer::Set(const SharedMediaClip& media)
{
    Refresh();

    m_Clip = media;
    m_Clip->SetColor(m_Color);

    m_PlayingComponent = PlayableComponent::Clip;
    UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());
    EnsureCached(media->FirstFrame());

    emit playlistUpdated(nullptr);
    CacheAvailable();
}

void ViewerBuffer::Set(const SharedPlaybackTrack& track)
{
    Refresh();

    m_Track = track;
    m_Track->SetColor(m_Color);

    m_PlayingComponent = PlayableComponent::Track;
    UpdateRange(m_Track->StartFrame(), m_Track->EndFrame());

    if (auto item = m_Track->GetTrackItem(m_Startframe))
        EnsureCached(item->StartFrame());

    CacheAvailable();
}

void ViewerBuffer::Set(const SharedPlaybackSequence& sequence)
{
    Refresh();

    m_Sequence = sequence;
    m_PlayingComponent = PlayableComponent::Sequence;
    UpdateRange(m_Sequence->StartFrame(), m_Sequence->EndFrame());

    if (auto item = m_Sequence->GetTrackItem(m_Startframe))
        EnsureCached(item->StartFrame());

    CacheAvailable();
}

void ViewerBuffer::Set(const std::vector<SharedMediaClip>& media)
{
    Refresh();

    m_Track->Clear();
    for (const SharedMediaClip& media : media)
        m_Track->AddMedia(media);

    m_PlayingComponent = PlayableComponent::Track;
    UpdateRange(m_Track->StartFrame(), m_Track->EndFrame());

    if (auto item = m_Track->GetTrackItem(m_Startframe))
        EnsureCached(item->StartFrame());

    CacheAvailable();
}

void ViewerBuffer::SetGrid(Playlist* playlist)
{
    Refresh();
    if (m_Playlist)
        disconnect(m_Playlist, &Playlist::updated, this, &ViewerBuffer::updated);

    m_Playlist = playlist;
    m_Clip = playlist->CurrentMedia();

    connect(m_Playlist, &Playlist::updated, this, &ViewerBuffer::updated);

    m_PlayingComponent = PlayableComponent::Grid;
    UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());

    emit playlistUpdated(playlist);

    EnsureCached(m_Clip->FirstFrame());
    CacheAvailable();
}

void ViewerBuffer::SetPlaylist(Playlist* playlist)
{
    Refresh();
    if (m_Playlist)
        disconnect(m_Playlist, &Playlist::updated, this, &ViewerBuffer::updated);

    m_Playlist = playlist;
    m_Clip = playlist->CurrentMedia();

    connect(m_Playlist, &Playlist::updated, this, &ViewerBuffer::updated);

    m_PlayingComponent = PlayableComponent::Playlist;
    UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());

    emit playlistUpdated(playlist);

    EnsureCached(m_Clip->FirstFrame());
    CacheAvailable();
}

void ViewerBuffer::SetColor(const QColor& color)
{
    /* Update the Buffer Color */
    m_Color = color;

    /* Update entities with the color change */
    m_Clip->SetColor(color);
    m_Track->SetColor(color);
}

SharedPlaybackTrack ViewerBuffer::ActiveTrack() const
{
    switch (m_PlayingComponent)
    {
        case PlayableComponent::Clip:
            /**
             * A clip is a separate entity and does not relate to track
             * If a clip is active then it should be the one currently playing and no track
             */
            return nullptr;
        case PlayableComponent::Track:
            /**
             * If the current playing component itself is a track, then it will be considered
             * as the active track, this could involve in adding elements to the active track
             */
            return m_Track;
        case PlayableComponent::Sequence:
            /**
             * If a sequence is currently playing, then this returns the currently active track
             * Meaning the track which is on the very top and also which is both enabled and visible
             * for it to recieve new entities and also show entities
             */
            /**
             * TODO: Need to think about, if this is really needed when right clicking > add media to sequence?
             * if added, where does the media go to? at the last of track or clears it?
             */
            return m_Sequence->ActiveVideoTrack();
        case PlayableComponent::Grid:
        case PlayableComponent::Playlist:
            return nullptr;
    }

    return nullptr;
}

void ViewerBuffer::Refresh()
{
    ClearCache();
}

void ViewerBuffer::ClearCache()
{
    StopCaching();

    m_Numbers.clear();
    m_Buffered.clear();

    if (m_PlayingComponent == PlayableComponent::Track)
        m_Track->ClearCache();
    else if (m_PlayingComponent == PlayableComponent::Sequence)
        m_Sequence->ClearCache();
    else
        m_Clip->Clear();
}

const FloatImage ViewerBuffer::Image(const v_frame_t frame)
{
    EnsureCached(frame);
    if (SharedMediaClip media = Media(frame))
        return media->Image(frame);
    
    return nullptr;
}

BufferData ViewerBuffer::MData(const v_frame_t frame, bool nearest)
{
    BufferData d;
    switch (m_PlayingComponent)
    {
        case PlayableComponent::Track:
            d.image = m_Track->Image(frame);
            break;
        case PlayableComponent::Sequence:
            d.image = m_Sequence->Image(frame);
            break;
        default:
            d.image = m_Clip->Image(frame);
            d.annotation = m_Clip->Annotation(frame);
    }

    return d;
}

std::vector<FloatImage> ViewerBuffer::GridFrame(const v_frame_t frame)
{
    std::vector<FloatImage> grid;
    grid.reserve(m_Playlist->Size());

    if (m_PlayingComponent == PlayableComponent::Grid)
    {
        for (auto& media : m_Playlist->AllMedia())
        {
            FloatImage image = VOID_NAMESPACE::Image<float>::Create();
            media->Image(media->Contains(frame) ? frame : media->NearestFrame(frame), image);
            grid.push_back(image);
        }
    }

    return grid;
}

SharedMediaClip ViewerBuffer::Media(const v_frame_t frame)
{
    switch (m_PlayingComponent)
    {
        case PlayableComponent::Sequence: return m_Sequence->Media(frame);
        case PlayableComponent::Track: return m_Track->Media(frame);
        case PlayableComponent::Grid:
        case PlayableComponent::Playlist:
        case PlayableComponent::Clip:
        default: return m_Clip;
    }
}

SharedTrackItem ViewerBuffer::TrackItem(const v_frame_t frame)
{
    return (m_PlayingComponent == PlayableComponent::Sequence)
            ? m_Sequence->GetTrackItem(frame)
            : m_Track->GetTrackItem(frame);
}

void ViewerBuffer::Clear()
{
    ClearCache();
    m_Clip = std::make_shared<MediaClip>();
}

bool ViewerBuffer::Playing(const SharedMediaClip& media) const
{
    // TODO: Check if we may have a use case around if the clip being compared against be a part of sequence or track as well
    return m_PlayingComponent == PlayableComponent::Clip ? media == m_Clip : false;
}

void ViewerBuffer::SetAnnotation(const v_frame_t frame, const Renderer::SharedAnnotation& annotation)
{
    /**
     * For now Just adding the annotation on the Media Clip
     * TODO: The Active sequence should also have a way to save Annotations to
     * that way it's not applied on to the TrackItem but on the sequence itself (or maybe check behaviours around this)
     */
    if (m_PlayingComponent == PlayableComponent::Clip)
        m_Clip->SetAnnotation(frame, annotation);
}

void ViewerBuffer::RemoveAnnotation(const v_frame_t frame)
{
    /**
     * For now Just removing the annotation from the Media Clip
     * TODO: The Active sequence should also have a way to save Annotations to and remove from
     * that way it's not happening on to TrackItem but on the sequence itself (or maybe check behaviours around this)
     */
    if (m_PlayingComponent == PlayableComponent::Clip)
        m_Clip->RemoveAnnotation(frame);
}

bool ViewerBuffer::NextMedia()
{
    if (m_PlayingComponent == PlayableComponent::Playlist)
    {
        Refresh();
        m_Clip = m_Playlist->NextMedia();

        UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());
        EnsureCached(m_Clip->FirstFrame());
        CacheAvailable();

        return true;
    }

    return false;
}

bool ViewerBuffer::PreviousMedia()
{
    if (m_PlayingComponent == PlayableComponent::Playlist)
    {
        Refresh();
        m_Clip = m_Playlist->PreviousMedia();

        UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());
        EnsureCached(m_Clip->FirstFrame());
        CacheAvailable();

        return true;
    }

    return false;
}

bool ViewerBuffer::ResetPlaylistMedia()
{
    /**
     * This is supposed to be invoked when we have reset the current index of the playlist
     * and we want that to be played now,
     * this clears existing cache and fetches the current media info from the playlist if available
     */
    if (m_PlayingComponent == PlayableComponent::Playlist)
    {
        Refresh();
        m_Clip = m_Playlist->CurrentMedia();

        UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());
        EnsureCached(m_Clip->FirstFrame());
        CacheAvailable();

        return true;
    }

    return false;
}

void ViewerBuffer::StartPlaybackCache(const PlayState& state)
{
    if (Completed() || m_State == PlayState::Disabled)
        return;

    m_State = state;
    m_CacheTimer.start(10);

    if (!m_ThreadPool.activeThreadCount() && !m_Numbers.empty())
        m_LastCached = m_State == PlayState::Forwards ? m_Numbers.back() : m_Numbers.front();
}

void ViewerBuffer::StopPlaybackCache()
{
    m_CacheTimer.stop();
}

void ViewerBuffer::PauseCaching()
{
    StopCaching();
    m_State = PlayState::Paused;
}

void ViewerBuffer::DisableCaching()
{
    StopCaching();
    m_State = PlayState::Disabled;
}

void ViewerBuffer::StopCaching()
{
    m_ThreadPool.clear();
    m_ThreadPool.waitForDone();

    m_Player->ClearCachedFrames();
    m_LastCached = m_Player->Frame();
}

void ViewerBuffer::ResumeCaching()
{
    m_State = PlayState::Forwards;
    CacheAvailable();
}

void ViewerBuffer::Update()
{
    // return;
    /**
     * Ensure that we do not have any other cache process running
     * or a cache process which is to spawn other cache processes
     */
    if (m_ThreadPool.activeThreadCount())
    {
        VOID_LOG_INFO("Already Caching");
        return;
    }

    if (m_State == PlayState::Forwards && !Completed())
        CacheNext();
    else if (m_State == PlayState::Backwards && !Completed())
        CachePrevious();
    else
        m_CacheTimer.stop();

    VOID_LOG_INFO("Update Cache.....");
}

void ViewerBuffer::UpdateRange(v_frame_t start, v_frame_t end)
{
    m_Startframe = start;
    m_Endframe = end;

    /**
     * The Back buffer refers to the amount of frames which stay behind the playhead
     * This needs to be between 3 - 10 depending on the overall size of the entity
     * being played
     */
    m_BackBuffer = std::min(10, std::max(3, static_cast<int>(((m_Endframe - m_Startframe) + 1) * 0.02)));
}

bool ViewerBuffer::Request(v_frame_t frame, bool evict)
{
    if (Completed())
    {
        m_CacheTimer.stop();
        return false;
    }

    if (Maxxed())
    {
        if (evict)
        {
            if (m_State == PlayState::Backwards)
            {
                EvictBack();
                m_Numbers.push_front(frame);
            }
            else
            {
                EvictFront();
                m_Numbers.push_back(frame);
            }

            return true;
        }

        // Cannot grant this request as we do not have enough memory
        return false;
    }

    m_State == PlayState::Backwards ? m_Numbers.push_front(frame) : m_Numbers.push_back(frame);
    return true;
}

void ViewerBuffer::SetFramesize(std::size_t size)
{
    if (size == m_Framesize)
        return;

    m_Framesize = size;
    m_Capacity = std::min(m_MaxMemory / m_Framesize, s_MaxBuffers);
    VOID_LOG_INFO("Setting New Frame size: {0}, Updated Capacity: {1}", m_Framesize, m_Capacity);
}

void ViewerBuffer::SetMaxMemory(std::size_t gigabytes)
{
    m_MaxMemory = gigabytes * 1024 * 1024 * 1024;
    m_Capacity = std::min(m_MaxMemory / m_Framesize, s_MaxBuffers);
    VOID_LOG_INFO("Setting New Frame size: {0}, Updated Capacity: {1}", m_Framesize, m_Capacity);
}

void ViewerBuffer::Cache(v_frame_t frame)
{
    if (m_PlayingComponent == PlayableComponent::Sequence)
    {
        const FloatImage& image = m_Sequence->Image(frame);
        SetFramesize(image->Size());

        Store(frame);
    }
    else if (m_PlayingComponent == PlayableComponent::Track)
    {
        const FloatImage& image = m_Track->Image(frame);
        SetFramesize(image->Size());

        Store(frame);
    }
    else if (m_Clip->Valid() && m_Clip->Contains(frame))
    {
        const FloatImage& image = m_Clip->Image(frame);
        SetFramesize(image->Size());

        Store(frame);
    }
}

void ViewerBuffer::EvictFront()
{
    v_frame_t frame = m_Numbers.front();
    switch (m_PlayingComponent)
    {
        case PlayableComponent::Track:
            m_Track->ClearCache(frame);
            break;
        case PlayableComponent::Sequence:
            m_Sequence->ClearCache(frame);
            break;
        case PlayableComponent::Clip:
        case PlayableComponent::Grid:
        case PlayableComponent::Playlist:
        default:
            if (m_Clip->Valid() && m_Clip->Contains(frame))
                m_Clip->Clear(frame);
    }

    m_Numbers.pop_front();
    m_Buffered.erase(frame);
    m_Player->RemoveCachedFrame(frame);
}

void ViewerBuffer::EvictBack()
{
    v_frame_t frame = m_Numbers.back();
    switch (m_PlayingComponent)
    {
        case PlayableComponent::Track:
            m_Track->ClearCache(frame);
            break;
        case PlayableComponent::Sequence:
            m_Sequence->ClearCache(frame);
            break;
        case PlayableComponent::Clip:
        case PlayableComponent::Grid:
        case PlayableComponent::Playlist:
        default:
            if (m_Clip->Valid() && m_Clip->Contains(frame))
                m_Clip->Clear(frame);
    }

    m_Numbers.pop_back();
    m_Buffered.erase(frame);
    m_Player->RemoveCachedFrame(frame);
}

void ViewerBuffer::Store(v_frame_t frame)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    m_Buffered.insert(frame);
    m_Player->AddCacheFrame(frame);
}

void ViewerBuffer::EnsureCached(v_frame_t frame)
{
    if (Buffered(frame))
        return;

    VOID_LOG_INFO("Force Caching Frame -- : {0}", frame);
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_LastCached = frame;
    }

    Request(frame, true);
    Cache(frame);
}

void ViewerBuffer::Recache()
{
    ClearCache();
    CacheAvailable();
}

void ViewerBuffer::CacheAvailable()
{
    if (m_State == PlayState::Disabled)
        return;

    /**
     * This is invoked whenever the media is set/changed on the player buffer
     * so the usual direction is Forwards, unless we were going backwards
     */
    if (m_State == PlayState::Backwards)
    {
        for (v_frame_t frame = m_Endframe; frame >= m_Startframe; --frame)
        {
            if (Buffered(frame))
                continue;

            // if (!Request(frame, false))
            //     break;

            if (Request(frame))
                AddTask(new CachePreviousFrameTask(this));
        }
    }
    else
    {
        for (v_frame_t frame = m_Startframe; frame <= m_Endframe; ++frame)
        {
            if (Buffered(frame))
                continue;

            if (Request(frame))
                AddTask(new CacheNextFrameTask(this));
        }
    }
}

v_frame_t ViewerBuffer::InternalStartframe() const
{
    return (m_Player) ? std::max(m_Startframe, m_Player->Startframe()) : m_Startframe;
}

v_frame_t ViewerBuffer::InternalEndframe() const
{
    return (m_Player) ? std::min(m_Endframe, m_Player->Endframe()) : m_Endframe;
}

int ViewerBuffer::InternalDuration() const
{
    // return InternalEndframe() - InternalStartframe() + 1;
    return m_Endframe - m_Startframe + 1;
}

bool ViewerBuffer::Completed() const
{
    /**
     * The cache/buffer is said to be completed if the internal frame store has the start
     * and the end frames present
     *
     */
    return m_Numbers.size() >= InternalDuration();
}

v_frame_t ViewerBuffer::GetNextFrame()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    v_frame_t frame = 0;

    if (m_LastCached >= m_Endframe)
        frame = m_Startframe;
    else
        frame = ++m_LastCached;

    m_LastCached = frame;

    return frame;
}

v_frame_t ViewerBuffer::GetPreviousFrame()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    v_frame_t frame = 0;

    if (m_LastCached <= m_Startframe)
        frame = m_Endframe;
    else
        frame = --m_LastCached;

    /* Update the last cached so that the next frame is the next to this */
    m_LastCached = frame;

    return frame;
}

void ViewerBuffer::CacheNextFrame()
{
    Cache(GetNextFrame());
}

void ViewerBuffer::CachePreviousFrame()
{
    Cache(GetPreviousFrame());
}

void ViewerBuffer::CacheNext()
{
    v_frame_t frame = m_LastCached;
    // int count = 0;

    // Remove and Request for new frames
    while (!m_Numbers.empty())
    {
        // // This might be a good idea to keep? in case all the frames are cached?
        // if (Completed())
        //     break;

        // Ensure the cached frame is just between the current frame and the back buffer
        if (m_Player->Frame() - m_BackBuffer <= m_Numbers.front() && m_Numbers.front() <= m_Player->Frame())
            break;

        frame++;
        // count++;
        // if (count > InternalDuration())
        //     break;

        if (frame > InternalEndframe())
            frame = InternalStartframe();

        if (Request(frame, true))
            AddTask(new CacheNextFrameTask(this));
    }
}

void ViewerBuffer::CachePrevious()
{
    v_frame_t frame = m_LastCached;

    // Remove and Request for new frames
    while (!m_Numbers.empty())
    {
        // Ensure the cached frame is just between the current frame and the back buffer
        if (m_Player->Frame() + m_BackBuffer >= m_Numbers.back() && m_Numbers.back() >= m_Player->Frame())
            break;

        frame--;

        if (frame < InternalStartframe())
            frame = InternalEndframe();

        Request(frame, true);
        AddTask(new CachePreviousFrameTask(this));
    }
}

void ViewerBuffer::SettingsUpdated()
{
    SetMaxMemory(VoidPreferences::Instance().GetCacheMemory());
    SetMaxThreads(VoidPreferences::Instance().GetCacheThreads());

    VOID_LOG_INFO("Cache Settings Updated.");
}

VOID_NAMESPACE_CLOSE
