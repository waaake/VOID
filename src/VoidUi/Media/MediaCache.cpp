// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include <iostream>

/* Internal */
#include "MediaCache.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"
#include "VoidCore/VoidTools.h"
#include "VoidUi/PlayerWidget.h"

VOID_NAMESPACE_OPEN

std::ostream& operator<<(std::ostream& stream, const std::deque<v_frame_t>& d)
{
    stream << "[";

    for (v_frame_t frame : d)
    {
        stream << frame << ", ";
    }

    stream << "]";

    return stream;
}

// int ForwardDistance(const std::deque<v_frame_t>& d, v_frame_t a, v_frame_t b)
// {
//     auto itA = std::lower_bound(d.cbegin(), d.cend(), a);
//     auto itB = std::find(d.cbegin(), d.cend(), b);

//     return std::distance<std::deque<v_frame_t>::const_iterator>(itA, itB);
// }

// int ReverseDistance(const std::deque<v_frame_t>& d, v_frame_t a, v_frame_t b)
// {
//     auto itA = std::find(d.cbegin(), d.cend(), a);
//     if (itA == d.end())
//     {
//         itA = std::upper_bound(d.cbegin(), d.cend(), a);
//     }

//     auto itB = std::find(d.cbegin(), d.cend(), b);

//     v_frame_t from = std::distance<std::deque<v_frame_t>::const_iterator>(d.cbegin(), itA);
//     v_frame_t to = std::distance<std::deque<v_frame_t>::const_iterator>(d.cbegin(), itB);

//     return (from - to + d.size()) % d.size();
// }

ChronoFlux::ChronoFlux(QObject* parent)
    : QObject(parent)
    , m_CacheDirection(Direction::None)
    , m_MaxMemory(1 * 1024 * 1024 * 1024) // 1 GB by default
    , m_UsedMemory(0)
    , m_FrameSize(0)
    , m_StartFrame(0)
    , m_EndFrame(0)
    , m_Duration(1)
    , m_LastCached(0)
{
    m_ThreadPool.setMaxThreadCount(5);

    connect(&m_CacheTimer, &QTimer::timeout, this, &ChronoFlux::Update);
}

ChronoFlux::~ChronoFlux()
{
}

void ChronoFlux::StartPlaybackCache(const Direction& direction)
{
    if (m_Framenumbers.size() >= m_Duration)
    {
        return;
    }

    m_CacheDirection = direction;
    m_CacheTimer.start(1000);

    /* Update the last frame for the Cache process to begin from */
    m_LastCached = m_CacheDirection == Direction::Forwards ? m_Framenumbers.back() : m_Framenumbers.front();
}

void ChronoFlux::StopPlaybackCache()
{
    m_CacheDirection = Direction::None;
    m_CacheTimer.stop();
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
        CacheNextq();
    else if (m_CacheDirection == Direction::Backwards)
        CachePreviousq();
    else
        m_CacheTimer.stop();

    VOID_LOG_INFO("Update Cache.....");
}

void ChronoFlux::SetMedia(const SharedMediaClip& media)
{
    m_Media = media;

    m_StartFrame = m_Media->FirstFrame();
    m_EndFrame = m_Media->LastFrame();
    m_Duration = m_Media->Duration();

    /**
     * Ensure that the first frame is cached
     * This helps with the understanding of the frame size of the media
     */
    EnsureCached(m_Media->FirstFrame());

    m_FrameSize = m_Media->FrameSize();
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
    if (m_Media->HasFrame(frame))
    {
        m_Media->CacheFrame(frame);

        /* Mark that the frame has been cached now */
        m_Player->AddCacheFrame(frame);
    }
}

void ChronoFlux::Evict(v_frame_t frame)
{
    auto it = std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame);

    if (it != m_Framenumbers.end())
    {
        m_Media->UncacheFrame(frame);
        m_UsedMemory -= m_FrameSize;

        m_Framenumbers.erase(it);
        m_Player->RemoveCachedFrame(frame);
    }
}

void ChronoFlux::EvictFront()
{
    v_frame_t frame = m_Framenumbers.front();

    m_Media->UncacheFrame(frame);
    m_UsedMemory -= m_FrameSize;

    m_Framenumbers.pop_front();
    m_Player->RemoveCachedFrame(frame);
}

void ChronoFlux::EvictBack()
{
    v_frame_t frame = m_Framenumbers.back();

    m_Media->UncacheFrame(frame);
    m_UsedMemory -= m_FrameSize;

    m_Framenumbers.pop_back();
    m_Player->RemoveCachedFrame(frame);
}

void ChronoFlux::EnsureCached(v_frame_t frame)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    /* Evict a frame if necessary as this needs to be cached */
    if (std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame) == m_Framenumbers.end())
    {
        std::cout << m_Framenumbers << std::endl;

        Request(frame, true);
        Cache(frame);
    }
}

void ChronoFlux::CacheAvailable()
{
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
        for (v_frame_t frame = std::max(m_LastCached, m_StartFrame); frame <= m_EndFrame; ++frame)
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

void ChronoFlux::CacheNextq()
{
    /* Determine how many frames are used up and can be removed */
    int count = Distance((m_Player->Frame() - 15), m_Framenumbers.front(), m_Duration);

    /* Cannot proceed */
    if (count < 1)
        return;

    v_frame_t frame = m_LastCached;

    for (int i = 0; i < count; ++i)
    {
        frame++;

        if (frame > m_EndFrame)
            frame = m_StartFrame;

        Request(frame, true);
        AddTask(new CacheNextFrameTask(this));
    }
}

void ChronoFlux::CachePreviousq()
{
    /* Determine how many frames are used up and can be removed */
    int count = Distance(m_Framenumbers.back(), (m_Player->Frame() + 15), m_Duration);

    /* Cannot proceed */
    if (count < 1)
        return;

    v_frame_t frame = m_LastCached;

    for (int i = 0; i < count; ++i)
    {
        frame--;

        if (frame < m_StartFrame)
            frame = m_EndFrame;

        Request(frame, true);
        AddTask(new CachePreviousFrameTask(this));
    }
}

v_frame_t ChronoFlux::CurrentFrame() const
{
    if (m_Player)
        return m_Player->Frame();

    return 0;
}

VOID_NAMESPACE_CLOSE
