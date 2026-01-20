// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include <iostream>

/* Internal */
#include "MediaCache.h"
#include "VoidCore/Logging.h"
#include "VoidCore/VoidTools.h"
#include "VoidUi/Player/Player.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

// std::ostream& operator<<(std::ostream& stream, const std::deque<v_frame_t>& d)
// {
//     stream << "[";

//     for (v_frame_t frame : d)
//     {
//         stream << frame << ", ";
//     }

//     stream << "]";

//     return stream;
// }

ChronoFlux::ChronoFlux(QObject* parent)
    : QObject(parent)
    , m_TrackView(nullptr)
    , m_SequenceView(nullptr)
    , m_CacheDirection(Direction::None)
    , m_State(State::Enabled)
    , m_CacheEntity(Entity::Media)
    , m_MaxMemory(VoidPreferences::Instance().GetCacheMemory() * 1024 * 1024 * 1024) // 1 GB by default
    , m_UsedMemory(0)
    , m_FrameSize(0)
    , m_StartFrame(0)
    , m_EndFrame(0)
    , m_Duration(1)
    , m_LastCached(0)
    , m_BackBuffer(3)
{
    m_ThreadPool.setMaxThreadCount(VoidPreferences::Instance().GetCacheThreads());

    connect(&m_CacheTimer, &QTimer::timeout, this, &ChronoFlux::Update);
    connect(&VoidPreferences::Instance(), &VoidPreferences::updated, this, &ChronoFlux::SettingsUpdated);
}

ChronoFlux::~ChronoFlux()
{
    ClearCache();

    ClearTrackView();
    ClearSequenceView();
}

void ChronoFlux::StartPlaybackCache(const Direction& direction)
{
    m_CacheDirection = direction;

    if (m_Framenumbers.size() >= m_Duration || m_State != State::Enabled)
        return;

    m_CacheTimer.start(10);

    if (!m_ThreadPool.activeThreadCount() && !m_Framenumbers.empty())
    {
        /* Update the last frame for the Cache process to begin from */
        m_LastCached = m_CacheDirection == Direction::Forwards ? m_Framenumbers.back() : m_Framenumbers.front();
    }
}

void ChronoFlux::StopPlaybackCache()
{
    m_CacheDirection = Direction::None;
    m_CacheTimer.stop();
}

void ChronoFlux::PauseCaching()
{
    StopCaching();
    m_State = State::Paused;
}

void ChronoFlux::DisableCaching()
{
    StopCaching();
    m_State = State::Disabled;
}

void ChronoFlux::StopCaching()
{
    /* Clear all the function threads that have not yet started */
    m_ThreadPool.clear();
    /* Wait for the remaining to be done */
    m_ThreadPool.waitForDone();

    m_Player->ClearCachedFrames();
    m_LastCached = m_Player->Frame();
}

void ChronoFlux::ResumeCaching()
{
    m_State = State::Enabled;
    CacheAvailable();
}

void ChronoFlux::ClearTrackView()
{
    if (m_TrackView)
    {
        m_TrackView->deleteLater();
        delete m_TrackView;
        m_TrackView = nullptr;
    }
}

void ChronoFlux::ClearSequenceView()
{
    if (m_SequenceView)
    {
        m_SequenceView->deleteLater();
        delete m_SequenceView;
        m_SequenceView = nullptr;
    }
}

void ChronoFlux::Update()
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

    else if (m_CacheDirection == Direction::Forwards && m_Framenumbers.size() != m_Duration)
        CacheNext();
    else if (m_CacheDirection == Direction::Backwards && m_Framenumbers.size() != m_Duration)
        CachePrevious();
    else
        m_CacheTimer.stop();

    VOID_LOG_INFO("Update Cache.....");
}

void ChronoFlux::UpdateRange(v_frame_t start, v_frame_t end)
{
    m_StartFrame = start;
    m_EndFrame = end;
    m_Duration = (m_EndFrame - m_StartFrame) + 1;
    m_Player->ReserveRange(m_Duration);

    /**
     * The Back buffer refers to the amount of frames which stay behind the playhead
     * This needs to be between 3 - 10 depending on the overall size of the entity
     * being played
     */
    m_BackBuffer = std::min(10, std::max(3, static_cast<int>(m_Duration * 0.02)));
}

void ChronoFlux::SetMedia(const SharedMediaClip& media)
{
    m_CacheEntity = Entity::Media;
    /* Clear Existing Media cache if present */
    ClearCache();

    m_Media = media;
    UpdateRange(media->FirstFrame(), media->LastFrame());

    /**
     * Ensure that the first frame is cached
     * This helps with the understanding of the frame size of the media
     */
    EnsureCached(media->FirstFrame());

    /**
     * Cache all of the available frames what the memory limit allows to
     */
    CacheAvailable();

    /* If the Cache was paused, it can be resumed now */
    if (m_State == State::Paused)
        m_State = State::Enabled;
}

void ChronoFlux::SetTrack(const SharedPlaybackTrack& track)
{
    m_CacheEntity = Entity::Track;
    /* Clear Existing Media cache if present */
    ClearCache();
    ClearTrackView();    

    m_TrackView = new TrackView(track);
    UpdateRange(track->StartFrame(), track->EndFrame());

    /**
     * Ensure that the first frame is cached
     * This helps with the understanding of the frame size of the media
     */
    EnsureCached(track->StartFrame());

    /**
     * Cache all of the available frames what the memory limit allows to
     */
    CacheAvailable();

    /* If the Cache was paused, it can be resumed now */
    if (m_State == State::Paused)
        m_State = State::Enabled;
}

void ChronoFlux::SetSequence(const SharedPlaybackSequence& sequence)
{
    m_CacheEntity = Entity::Sequence;
    /* Clear Existing Media cache if present */
    ClearCache();
    ClearSequenceView();

    m_SequenceView = new SequenceView(sequence);
    UpdateRange(sequence->StartFrame(), sequence->EndFrame());

    /**
     * Ensure that the first frame is cached
     * This helps with the understanding of the frame size of the media
     */
    EnsureCached(sequence->StartFrame());

    /**
     * Cache all of the available frames what the memory limit allows to
     */
    CacheAvailable();

    /* If the Cache was paused, it can be resumed now */
    if (m_State == State::Paused)
        m_State = State::Enabled;
}

bool ChronoFlux::Request(v_frame_t frame, bool evict)
{
    /**
     * Size isn't yet set so we can definitely go for caching the first frame
     * well, unless the first frame itself is more than the max available memory
     * (highly unlikely unless we're on 256 MB allocation)
     */
    if (!m_FrameSize)
    {
        if (m_CacheDirection == Direction::Backwards)
            m_Framenumbers.push_front(frame);
        else
            m_Framenumbers.push_back(frame);

        return true;
    }

    /**
     * Check if we've cached all of our frames
     * if so, then there is no need to cache any further
     */
    if (m_Framenumbers.size() >= m_Duration)
    {
        m_CacheTimer.stop();

        return false;
    }

    if (m_FrameSize > AvailableMemory())
    {
        if (evict)
        {
            if (m_CacheDirection == Direction::Backwards)
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

    if (m_CacheDirection == Direction::Backwards)
        m_Framenumbers.push_front(frame);
    else
        m_Framenumbers.push_back(frame);

    return true;
}

void ChronoFlux::Cache(v_frame_t frame)
{
    if (m_CacheEntity == Entity::Track)
    {
        SharedTrackItem item = m_TrackView->ItemAt(frame);

        if (!item)
            return;

        item->CacheFrame(frame);
        m_FrameSize = item->FrameSize();

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            /* Mark that the frame has been cached now */
            m_Player->AddCacheFrame(frame);
        }

        return;
    }

    if (m_CacheEntity == Entity::Sequence)
    {
        SharedTrackItem item = m_SequenceView->ItemAt(frame);

        if (!item)
            return;

        item->CacheFrame(frame);
        m_FrameSize = item->FrameSize();

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            /* Mark that the frame has been cached now */
            m_Player->AddCacheFrame(frame);
        }

        return;
    }

    if (SharedMediaClip media = m_Media.lock())
    {
        if (media->Valid() && media->Contains(frame))
        {
            media->CacheFrame(frame);
            m_FrameSize = media->FrameSize();

            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                /* Mark that the frame has been cached now */
                m_Player->AddCacheFrame(frame);
            }
        }
    }
}

void ChronoFlux::Evict(v_frame_t frame)
{
    auto it = std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame);

    if (SharedMediaClip media = m_Media.lock())
    {
        if (it != m_Framenumbers.end())
        {
            media->UncacheFrame(frame);
            m_UsedMemory -= m_FrameSize;

            m_Framenumbers.erase(it);
            m_Player->RemoveCachedFrame(frame);
        }
    }
}

void ChronoFlux::EvictFront()
{
    v_frame_t frame = m_Framenumbers.front();

    switch (m_CacheEntity)
    {
        case Entity::Track:
            m_TrackView->ItemAt(frame)->UncacheFrame(frame);
        case Entity::Sequence:
            m_SequenceView->ItemAt(frame)->UncacheFrame(frame);
        case Entity::Media:
        default:
        {
            if (SharedMediaClip media = m_Media.lock())
                media->UncacheFrame(frame);
        }
    }

    m_UsedMemory -= m_FrameSize;
    m_Framenumbers.pop_front();
    m_Player->RemoveCachedFrame(frame);
}

void ChronoFlux::EvictBack()
{
    v_frame_t frame = m_Framenumbers.back();

    switch (m_CacheEntity)
    {
        case Entity::Track:
            m_TrackView->ItemAt(frame)->UncacheFrame(frame);
        case Entity::Sequence:
            m_SequenceView->ItemAt(frame)->UncacheFrame(frame);
        case Entity::Media:
        default:
        {
            if (SharedMediaClip media = m_Media.lock())
                media->UncacheFrame(frame);
        }
    }

    m_UsedMemory -= m_FrameSize;
    m_Framenumbers.pop_back();
    m_Player->RemoveCachedFrame(frame);
}

void ChronoFlux::EnsureCached(v_frame_t frame)
{
    if (std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame) == m_Framenumbers.end())
    {
        VOID_LOG_INFO("Force Caching Frame: {0}", frame);
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_LastCached = frame;
        }

        /* Evict a frame if necessary as this needs to be cached */
        Request(frame, true);
        Cache(frame);
    }
}

void ChronoFlux::ClearCache()
{

    if (m_CacheEntity == Entity::Track && m_TrackView)
        m_TrackView->Track()->ClearCache();
    else if (m_CacheEntity == Entity::Sequence && m_SequenceView)
        m_SequenceView->Sequence()->ClearCache();
    else if (SharedMediaClip media = m_Media.lock())
        media->ClearCache();

    m_Framenumbers.clear();
    m_UsedMemory = 0;

    StopCaching();
}

void ChronoFlux::Recache()
{
    ClearCache();
    CacheAvailable();
}

void ChronoFlux::CacheAvailable()
{
    if (m_State == State::Disabled)
        return;

    int count = 0;

    /**
     * This is invoked whenever the media is set/changed on the player buffer
     * so the usual direction is Forwards, unless we were going backwards
     */
    if (m_CacheDirection == Direction::Backwards)
    {
        for (v_frame_t frame = m_EndFrame; frame >= m_StartFrame; --frame)
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
        for (v_frame_t frame = m_StartFrame; frame <= m_EndFrame; ++frame)
        {
            if (Cached(frame))
                continue;

            if (!Request(frame, false))
                break;

            AddTask(new CacheNextFrameTask(this));
        }
    }
}

v_frame_t ChronoFlux::GetNextFrame()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    v_frame_t frame = 0;

    if (m_LastCached > m_EndFrame)
        frame = m_StartFrame;
    else
        frame = ++m_LastCached;

    m_LastCached = frame;

    return frame;
}

v_frame_t ChronoFlux::GetPreviousFrame()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    v_frame_t frame = 0;

    if (m_LastCached <= m_StartFrame)
        frame = m_EndFrame;
    else
        frame = --m_LastCached;

    /* Update the last cached so that the next frame is the next to this */
    m_LastCached = frame;

    return frame;
}

void ChronoFlux::CacheNextFrame()
{
    Cache(GetNextFrame());
}

void ChronoFlux::CachePreviousFrame()
{
    Cache(GetPreviousFrame());
}

void ChronoFlux::CacheNext()
{
    v_frame_t frame = m_LastCached;
    
    /* Determine how many frames are used up and can be removed */
    while (!m_Framenumbers.empty())
    {
        /* Ensure the cached frame is just between the current frame and the back buffer */
        if (m_Player->Frame() - m_BackBuffer < m_Framenumbers.front() && m_Framenumbers.front() <= m_Player->Frame())
            break;

        frame++;

        if (frame > m_EndFrame)
            frame = m_StartFrame;

        Request(frame, true);
        AddTask(new CacheNextFrameTask(this));
    }
}

void ChronoFlux::CachePrevious()
{
    v_frame_t frame = m_LastCached;

    /* Determine how many frames are used up and can be removed */
    while (!m_Framenumbers.empty())
    {
        /* Ensure the cached frame is just between the current frame and the back buffer */
        if (m_Player->Frame() + m_BackBuffer > m_Framenumbers.back() && m_Framenumbers.back() >= m_Player->Frame())
            break;

        frame--;

        if (frame < m_StartFrame)
            frame = m_EndFrame;

        Request(frame, true);
        AddTask(new CachePreviousFrameTask(this));
    }
}

void ChronoFlux::SettingsUpdated()
{
    SetMaxMemory(VoidPreferences::Instance().GetCacheMemory());
    SetMaxThreads(VoidPreferences::Instance().GetCacheThreads());

    VOID_LOG_INFO("Cache Settings Updated.");
}

VOID_NAMESPACE_CLOSE
