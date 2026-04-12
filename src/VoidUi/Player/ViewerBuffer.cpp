// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ViewerBuffer.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Player/Player.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

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
    , m_Clip(std::make_shared<MediaClip>())
    , m_Track(std::make_shared<PlaybackTrack>())
    , m_Sequence(std::make_shared<PlaybackSequence>())
    , m_Playlist(nullptr)
    , m_CachedTrackItem(nullptr)
    , m_PlayingComponent(PlayableComponent::Clip)
    , m_State(PlayState::Forwards)
    , m_Name("Viewer")
    , m_Color(130, 110, 190)    // Purple
    , m_Player(nullptr)
    , m_MaxMemory(VoidPreferences::Instance().GetCacheMemory() * 1024 * 1024 * 1024) // 1 GB by default
    , m_UsedMemory(0)
    , m_FrameSize(0)
    , m_Startframe(0)
    , m_Endframe(1)
    , m_LastCached(0)
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
    CacheAvailable();
}

void ViewerBuffer::Set(const SharedPlaybackTrack& track)
{
    Refresh();

    m_Track = track;
    m_Track->SetColor(m_Color);

    m_PlayingComponent = PlayableComponent::Track;
    UpdateRange(m_Track->StartFrame(), m_Track->EndFrame());

    if (auto item = ItemFromTrack(m_Startframe))
        EnsureCached(item->StartFrame());

    CacheAvailable();
}

void ViewerBuffer::Set(const SharedPlaybackSequence& sequence)
{
    Refresh();

    m_Sequence = sequence;
    m_PlayingComponent = PlayableComponent::Sequence;
    UpdateRange(m_Sequence->StartFrame(), m_Sequence->EndFrame());

    if (auto item = ItemFromTrack(m_Startframe))
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

    if (auto item = ItemFromTrack(m_Startframe))
        EnsureCached(item->StartFrame());

    CacheAvailable();
}

void ViewerBuffer::SetPlaylist(Playlist* playlist)
{
    Refresh();

    m_Playlist = playlist;
    m_Clip = playlist->CurrentMedia();

    m_PlayingComponent = PlayableComponent::Playlist;
    UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());

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
        case PlayableComponent::Playlist:
            return nullptr;
    }

    return nullptr;
}

void ViewerBuffer::Refresh()
{
    /* Clear any cached item */
    m_CachedTrackItem = nullptr;
    ClearCache();
}

void ViewerBuffer::ClearCache()
{
    StopCaching();

    m_Framenumbers.clear();
    m_Buffered.clear();
    m_UsedMemory = 0;

    if (m_PlayingComponent == PlayableComponent::Track)
        m_Track->ClearCache();
    else if (m_PlayingComponent == PlayableComponent::Sequence)
        m_Sequence->ClearCache();
    else
        m_Clip->ClearCache();
}

SharedPixels ViewerBuffer::Image(const v_frame_t frame)
{
    /* The active element is a clip */
    if (m_PlayingComponent == PlayableComponent::Clip)
    {
        if (m_Clip->Empty() || !m_Clip->InRange(frame))
            return nullptr;

        return m_Clip->Image(frame);
    }

    SharedTrackItem item = ItemFromTrack(frame);

    if (!item)
        return nullptr;

    /* The pixels from the track item */
    return item->Image(frame);
}

BufferData ViewerBuffer::MData(const v_frame_t frame, bool nearest)
{
    switch (m_PlayingComponent)
    {
        case PlayableComponent::Track: return TrackData(frame, nearest);
        case PlayableComponent::Sequence: return SequenceData(frame, nearest);
        default: return ClipData(frame, nearest);
    }
}

SharedMediaClip ViewerBuffer::Media(const v_frame_t frame)
{
    if (m_PlayingComponent == PlayableComponent::Track)
    {
        if (SharedTrackItem item = ItemFromTrack(frame))
            return item->GetMedia();
        return nullptr;
    }

    if (m_PlayingComponent == PlayableComponent::Sequence)
    {
        if (SharedTrackItem item = ItemFromSequence(frame))
            return item->GetMedia();
        return nullptr;
    }

    return m_Clip;
}

BufferData ViewerBuffer::ClipData(const v_frame_t frame, bool nearest)
{
    BufferData d;
    if (m_Clip->InRange(frame) && m_Clip->Contains(frame))
    {
        EnsureCached(frame);
        d.image = m_Clip->Image(frame);
        d.annotation = m_Clip->Annotation(frame);
        return d;
    }

    return (nearest) ? ClipData(m_Clip->NearestFrame(frame), false) : d;
}

BufferData ViewerBuffer::TrackData(const v_frame_t frame, bool nearest)
{
    BufferData d;
    SharedTrackItem item = ItemFromTrack(frame);
    if (item && item ->InRange(frame))
    {
        EnsureCached(frame);
        d.image = item->Image(frame);
        return d;
    }

    return d;
}

BufferData ViewerBuffer::SequenceData(const v_frame_t frame, bool nearest)
{
    BufferData d;
    SharedTrackItem item = ItemFromSequence(frame);
    if (item && item->InRange(frame))
    {
        EnsureCached(frame);
        d.image = item->Image(frame);
        return d;
    }

    return d;
}

SharedTrackItem ViewerBuffer::ItemFromSequence(const v_frame_t frame)
{
    /**
     * Check if we have a cached track item present -> return that
     * if the cached track item is not preset or if the item does not have the requested frame
     * request for a track item from the track at the frame, cache it and return back
     */
    if (!m_CachedTrackItem || !m_CachedTrackItem->InRange(frame))
    {
        /* Cache the item from the Sequence */
        m_CachedTrackItem = m_Sequence->GetTrackItem(frame);
    }

    /* Return what has been cached */
    return m_CachedTrackItem;
}

SharedTrackItem ViewerBuffer::ItemFromTrack(const v_frame_t frame)
{
    /**
     * Check if we have a cached track item present -> return that
     * if the cached track item is not preset or if the item does not have the requested frame
     * request for a track item from the track at the frame, cache it and return back
     */
    if (!m_CachedTrackItem || !m_CachedTrackItem->InRange(frame))
    {
        /* Cache the item from the track */
        m_CachedTrackItem = m_Track->GetTrackItem(frame);
    }

    /* Return what has been cached */
    return m_CachedTrackItem;
}

SharedTrackItem ViewerBuffer::TrackItem(const v_frame_t frame)
{
    return (m_PlayingComponent == PlayableComponent::Sequence) ? ItemFromSequence(frame) : ItemFromTrack(frame);
}

void ViewerBuffer::Clear()
{
    /* Clear up --> Point to an empty clip */
    m_Clip = std::make_shared<MediaClip>();
    /* Clear Cache */
    ClearCache();
}

bool ViewerBuffer::Playing(const SharedMediaClip& media) const
{
    /* If a clip is being played check if the internal clip and the media are same */
    if (m_PlayingComponent == PlayableComponent::Clip)
        return media == m_Clip;

    /**
     * In other cases the cached track item should tell us if the clip was being played currently
     */
    return m_CachedTrackItem->GetMedia() == media;
}

void ViewerBuffer::SetAnnotation(const v_frame_t frame, const Renderer::SharedAnnotation& annotation)
{
    /**
     * For now Just adding the annotation on the Media Clip
     * TODO: The Active sequence should also have a way to save Annotations to
     * that way it's not applied on to the TrackItem but on the sequence itself (or maybe check behaviours around this)
     */
    if (m_PlayingComponent == PlayableComponent::Clip)
    {
        /* Save the annotation on the Media clip */
        m_Clip->SetAnnotation(frame, annotation);
    }
}

void ViewerBuffer::RemoveAnnotation(const v_frame_t frame)
{
    /**
     * For now Just removing the annotation from the Media Clip
     * TODO: The Active sequence should also have a way to save Annotations to and remove from
     * that way it's not happening on to TrackItem but on the sequence itself (or maybe check behaviours around this)
     */
    if (m_PlayingComponent == PlayableComponent::Clip)
    {
        /* Remove the annotation from the Media clip on the given frame */
        m_Clip->RemoveAnnotation(frame);
    }
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

void ViewerBuffer::StartPlaybackCache(const PlayState& state)
{
    if (Completed() || m_State == PlayState::Disabled)
        return;

    m_State = state;
    m_CacheTimer.start(10);

    if (!m_ThreadPool.activeThreadCount() && !m_Framenumbers.empty())
        m_LastCached = m_State == PlayState::Forwards ? m_Framenumbers.back() : m_Framenumbers.front();
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
    /* Clear all the function threads that have not yet started */
    m_ThreadPool.clear();
    /* Wait for the remaining to be done */
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
    /**
     * Size isn't yet set so we can definitely go for caching the first frame
     * well, unless the first frame itself is more than the max available memory
     * (highly unlikely unless we're on 256 MB allocation)
     */
    if (!m_FrameSize)
    {
        m_State == PlayState::Backwards ? m_Framenumbers.push_front(frame) : m_Framenumbers.push_back(frame);
        return true;
    }

    /**
     * Check if we've cached all of our frames
     * if so, then there is no need to cache any further
     */
    if (evict && Completed())
    {
        m_CacheTimer.stop();
        return false;
    }

    if (m_FrameSize > AvailableMemory())
    {
        if (evict)
        {
            if (m_State == PlayState::Backwards)
            {
                EvictBack();
                m_Framenumbers.push_front(frame);
            }
            else
            {
                EvictFront();
                m_Framenumbers.push_back(frame);
            }

            m_UsedMemory += m_FrameSize;
            return true;
        }

        /* Cannot grant this request as we do not have enough memory */
        return false;
    }

    m_UsedMemory += m_FrameSize;
    m_State == PlayState::Backwards ? m_Framenumbers.push_front(frame) : m_Framenumbers.push_back(frame);
    return true;
}

void ViewerBuffer::Cache(v_frame_t frame)
{
    if (m_PlayingComponent == PlayableComponent::Track)
    {
        SharedTrackItem item = ItemFromTrack(frame);
        if (item)
        {
            item->CacheFrame(frame);
            m_FrameSize = item->FrameSize();

            Store(frame);
        }

        return;
    }

    if (m_PlayingComponent == PlayableComponent::Sequence)
    {
        if (SharedTrackItem item = ItemFromSequence(frame))
        {
            // auto image = item->Image(frame);
            // m_FrameSize = image->FrameSize();
            item->CacheFrame(frame);
            m_FrameSize = item->FrameSize();

            Store(frame);
        }

        return;
    }

    if (m_Clip->Valid() && m_Clip->Contains(frame))
    {
        // auto image = m_Clip->Image(frame);
        // m_FrameSize = image->FrameSize();
        m_Clip->CacheFrame(frame);
        m_FrameSize = m_Clip->FrameSize();

        Store(frame);
    }
}

void ViewerBuffer::EvictFront()
{
    v_frame_t frame = m_Framenumbers.front();

    switch (m_PlayingComponent)
    {
        case PlayableComponent::Track:
            ItemFromTrack(frame)->UncacheFrame(frame);
            break;
        case PlayableComponent::Sequence:
            ItemFromSequence(frame)->UncacheFrame(frame);
            break;
        case PlayableComponent::Clip:
        case PlayableComponent::Playlist:
        default:
            if (m_Clip->Valid() && m_Clip->Contains(frame))
                m_Clip->UncacheFrame(frame);
    }

    m_UsedMemory -= m_FrameSize;

    m_Framenumbers.pop_front();
    m_Buffered.erase(frame);
    m_Player->RemoveCachedFrame(frame);
}

void ViewerBuffer::EvictBack()
{
    v_frame_t frame = m_Framenumbers.back();

    switch (m_PlayingComponent)
    {
        case PlayableComponent::Track:
            ItemFromTrack(frame)->UncacheFrame(frame);
            break;
        case PlayableComponent::Sequence:
            ItemFromSequence(frame)->UncacheFrame(frame);
            break;
        case PlayableComponent::Clip:
        case PlayableComponent::Playlist:
        default:
            if (m_Clip->Valid() && m_Clip->Contains(frame))
                m_Clip->UncacheFrame(frame);
    }

    m_UsedMemory -= m_FrameSize;
    m_Framenumbers.pop_back();
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
    if (m_Buffered.find(frame) == m_Buffered.end())
    {
        VOID_LOG_INFO("Force Caching Frame: {0}", frame);
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_LastCached = frame;
        }

        Request(frame, true);
        Cache(frame);
    }
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
            if (Cached(frame))
                continue;

            if (!Request(frame, false))
                break;

            AddTask(new CachePreviousFrameTask(this));
        }
    }
    else
    {
        for (v_frame_t frame = m_Startframe; frame <= m_Endframe; ++frame)
        {
            if (Cached(frame))
                continue;

            if (!Request(frame, false))
                break;

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
    return m_Framenumbers.size() >= InternalDuration();
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

    /* Determine how many frames are used up and can be removed */
    while (!m_Framenumbers.empty())
    {
        // This might be a good idea to keep? in case all the frames are cached?
        // if (Completed())
        //     break;

        /* Ensure the cached frame is just between the current frame and the back buffer */
        if (m_Player->Frame() - m_BackBuffer < m_Framenumbers.front() && m_Framenumbers.front() <= m_Player->Frame())
            break;

        frame++;

        if (frame > InternalEndframe())
            frame = InternalStartframe();

        Request(frame, true);
        AddTask(new CacheNextFrameTask(this));
    }
}

void ViewerBuffer::CachePrevious()
{
    v_frame_t frame = m_LastCached;

    /* Determine how many frames are used up and can be removed */
    while (!m_Framenumbers.empty())
    {
        /* Ensure the cached frame is just between the current frame and the back buffer */
        if (m_Player->Frame() + m_BackBuffer >= m_Framenumbers.back() && m_Framenumbers.back() >= m_Player->Frame())
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
