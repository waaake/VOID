// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include <iostream>

/* Internal */
#include "MediaCache.h"
#include "VoidCore/Logging.h"
#include "VoidCore/VoidTools.h"
#include "VoidUi/PlayerWidget.h"
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
    , m_CacheDirection(Direction::None)
    , m_State(State::Enabled)
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
}

void ChronoFlux::StartPlaybackCache(const Direction& direction)
{
    if (m_Framenumbers.size() >= m_Duration || m_State != State::Enabled)
    {
        return;
    }

    m_CacheDirection = direction;
    m_CacheTimer.start(1000);

    if (!m_ThreadPool.activeThreadCount())
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

    if (m_CacheDirection == Direction::Forwards)
        CacheNext();
    else if (m_CacheDirection == Direction::Backwards)
        CachePrevious();
    else
        m_CacheTimer.stop();

    VOID_LOG_INFO("Update Cache.....");
}

void ChronoFlux::SetMedia(const SharedMediaClip& media)
{
    /* Clear Existing Media cache if present */
    ClearCache();

    m_Media = media;

    m_StartFrame = media->FirstFrame();
    m_EndFrame = media->LastFrame();
    m_Duration = media->Duration();

    m_BackBuffer = std::max(3, static_cast<int>(m_Duration * 0.02));

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
        m_CacheDirection = Direction::None;
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
    if (SharedMediaClip media = m_Media.lock())
    {
        if (media->HasFrame(frame))
        {
            media->CacheFrame(frame);
            m_FrameSize = media->FrameSize();

            /* Mark that the frame has been cached now */
            m_Player->AddCacheFrame(frame);
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

    if (SharedMediaClip media = m_Media.lock())
    {
        media->UncacheFrame(frame);
        m_UsedMemory -= m_FrameSize;

        m_Framenumbers.pop_front();
        m_Player->RemoveCachedFrame(frame);

        VOID_LOG_INFO("Removing Cached Frame: {0}", frame);
    }
}

void ChronoFlux::EvictBack()
{
    v_frame_t frame = m_Framenumbers.back();

    if (SharedMediaClip media = m_Media.lock())
    {
        media->UncacheFrame(frame);
        m_UsedMemory -= m_FrameSize;

        m_Framenumbers.pop_back();
        m_Player->RemoveCachedFrame(frame);
    }
}

void ChronoFlux::EnsureCached(v_frame_t frame)
{
    if (std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame) == m_Framenumbers.end())
    {
        // std::cout << m_Framenumbers << std::endl;
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
    if (SharedMediaClip media = m_Media.lock())
    {
        media->ClearCache();
    }

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
        if (m_Player->Frame() - m_BackBuffer < m_Framenumbers.front() && m_Framenumbers.front() < m_Player->Frame())
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
        if (m_Player->Frame() + m_BackBuffer > m_Framenumbers.back() && m_Framenumbers.back() > m_Player->Frame())
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
