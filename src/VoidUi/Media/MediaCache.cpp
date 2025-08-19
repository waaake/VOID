// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include <iostream>

/* Internal */
#include "MediaCache.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"
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

ChronoFlux::ChronoFlux(QObject* parent)
    : QObject(parent)
    , m_CacheDirection(Direction::Backwards)
    , m_MaxMemory(1 * 1024 * 1024 * 1024) // 1 GB by default
    , m_UsedMemory(0)
    , m_FrameSize(0)
    , m_StartFrame(0)
    , m_EndFrame(0)
    , m_Duration(1)
    , m_LastCached(0)
    , m_Cache(false)
{
    m_ThreadPool.setMaxThreadCount(5);

    connect(&m_ForwardsTimer, &QTimer::timeout, this, &ChronoFlux::Update);
}

ChronoFlux::~ChronoFlux()
{
}

void ChronoFlux::CacheAvailableFrames()
{
    AddTask(new CacheAvailableFramesTask(this));
}

void ChronoFlux::StartPlaybackCache()
{
    // m_Cache = true;
    // AddTask(new CachePlayingFramesTask(this));
    m_ForwardsTimer.start(1000);
}

void ChronoFlux::StopPlaybackCache()
{
    m_Cache = false;
    m_ForwardsTimer.stop();
}

void ChronoFlux::Update()
{
    if (m_CacheDirection == Direction::Forwards)
        CacheNextq();
    else
        CachePreviousq();
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
    // VOID_LOG_ERROR("-----------------------------------------------------------");
    // VOID_LOG_ERROR("Requested {0}", frame);
    // /* Cache already exists for the frame */
    bool existing = std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame) != m_Framenumbers.end();
    //     return false;

    /**
     * Size isn't yet set so we can definitely go for caching the first frame
     * well, unless the first frame itself is more than the max available memory
     * (highly unlikely unless we're on 256 MB allocation)
     */
    if (!m_FrameSize)
    {
        if (m_CacheDirection == Direction::Forwards)
            m_Framenumbers.push_back(frame);
        else
            m_Framenumbers.push_front(frame);

        return true;
    }

    if (m_FrameSize > AvailableMemory())
    {
        if (evict)
        {
            if (m_CacheDirection == Direction::Forwards)
            {
                EvictFront();
                m_Framenumbers.push_back(frame);
            }
            else
            {
                EvictBack();
                m_Framenumbers.push_front(frame);
            }
            m_UsedMemory += m_FrameSize;

            return true;
        }

        /* Cannot grant this request as we do not have enough memory */
        return false;
    }

    m_UsedMemory += m_FrameSize;
    if (m_CacheDirection == Direction::Forwards)
        m_Framenumbers.push_back(frame);
    else
        m_Framenumbers.push_front(frame);

    return true;
}

void ChronoFlux::Cache(v_frame_t frame)
{
    // VOID_LOG_INFO("C F: {0}", frame);
    // Tools::VoidProfiler<std::chrono::duration<double>> p("Cache Frame");

    // /**
    //  * Want to be double sure that we do not exceed Memory limit,
    //  * Although we should not end up in this condition anytime but just keeping for time this cache
    //  * engine is in it's infancy to see and catch any edge case that might just lead it here
    //  */
    // if (m_FrameSize > AvailableMemory())
    //     return;

    // /* Cache already exists for the frame */
    // if (std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame) != m_Framenumbers.end())
    //     return;

    if (m_Media->HasFrame(frame))
    {
        // m_LastCached = frame;

        // VOID_LOG_WARN("Cached -> {0}", frame);

        m_Media->CacheFrame(frame);

        // if (m_CacheDirection == Direction::Forwards)
        //     m_Framenumbers.push_back(frame);
        // else
        //     m_Framenumbers.push_front(frame);

        /* Since the frame has been cached */
        // m_FrameSize = m_Media->FrameSize();

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
        // {
        //     std::lock_guard<std::mutex> lock(m_Mutex);
        //     m_LastCached = frame;
        // }

        // VOID_LOG_INFO("ENSURED: {0}", frame);

        std::cout << m_Framenumbers << std::endl;

        // m_LastCached = frame;

        Request(frame, true);
        Cache(frame);
    }
}

void ChronoFlux::CacheAvailable()
{
    int count = 0;

    if (m_CacheDirection == Direction::Forwards)
    {
        for (v_frame_t frame = std::max(m_LastCached, m_StartFrame); frame <= m_EndFrame; ++frame)
        {
            if (!Request(frame, false))
                break;
    
            count++;
        }
    
        // VOID_LOG_INFO("Caching Next Frame: {0}", count);
        CacheNext(count, true);
    }
    else
    {
        for (v_frame_t frame = m_EndFrame; frame >= m_StartFrame; --frame)
        {
            if (!Request(frame, false))
                break;
    
            count++;
        }
    
        CachePrevious(count, true);
    }
}

v_frame_t ChronoFlux::GetNextFrame()
{
    // VOID_LOG_INFO("==========================");
    std::lock_guard<std::mutex> lock(m_Mutex);
    // VOID_LOG_INFO("Last Cached Frame : {0}", m_LastCached);
    v_frame_t frame = 0;

    if (!m_LastCached || m_LastCached > m_EndFrame)
    {
        frame = m_StartFrame;
        // VOID_LOG_INFO("A");
    }
    else
    {
        // frame = std::min(++m_LastCached, m_Framenumbers.back());
        frame = ++m_LastCached;
        // VOID_LOG_INFO("B");
    }

    /* Update the last cached so that the next frame is the next to this */
    m_LastCached = frame;

    // VOID_LOG_INFO("Queried Next Frame: {0}", frame);
    // VOID_LOG_INFO("==========================\n\n");

    // VOID_LOG_INFO("Last Cached: {0}--{1}", m_LastCached, frame);

    return frame;
}

v_frame_t ChronoFlux::GetPreviousFrame()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    v_frame_t frame = 0;

    if (!m_LastCached || m_LastCached <= m_StartFrame)
        frame = m_EndFrame;
    else
        frame = --m_LastCached;

    /* Update the last cached so that the next frame is the next to this */
    m_LastCached = frame;

    return frame;
}

void ChronoFlux::CacheNextFrame()
{
    // if (!m_LastCached || m_LastCached >= m_EndFrame)
    //     Cache(m_StartFrame);
    // else
    //     Cache(std::min(++m_LastCached, m_Framenumbers.back()));
    Cache(GetNextFrame());
}

void ChronoFlux::CachePreviousFrame()
{
    // if (!m_LastCached || m_LastCached <= m_StartFrame)
    //     Cache(m_EndFrame);
    // else
    //     Cache(std::max(--m_LastCached, m_Framenumbers.front()));
    Cache(GetPreviousFrame());
}

void ChronoFlux::CacheNextq()
{
    // VOID_LOG_INFO("=================================================");
    // VOID_LOG_INFO("Current : {0}", m_Player->Frame());
    // VOID_LOG_INFO("Current - 15 : {0}", m_Player->Frame() - 15);
    // VOID_LOG_INFO("Front : {0}", m_Framenumbers.front());
    // int count = 0;

    // /* Determine how many frames are used up and can be removed */
    // int preframe = m_Player->Frame() - 15;

    // if (preframe >= m_StartFrame)
    // {
    //     count = preframe - m_Framenumbers.front();
    // }
    // else
    // {
    //     count = m_StartFrame - preframe;
    // }

    // int current = ((m_Player->Frame() - 15) + m_Duration) % m_Duration;
    // int front = m_Framenumbers.front() % m_Duration;

    int count = Distance((m_Player->Frame() - 15), m_Framenumbers.front(), m_Duration);

    // int frame -

    // VOID_LOG_INFO("Count : {0}", count);

    /* Cannot proceed */
    if (count < 1)
        return;

    // VOID_LOG_INFO("Last Cached : {0}", m_LastCached);
    // VOID_LOG_INFO("Back : {0}", m_Framenumbers.back());

    int frame = m_LastCached;

    for (int i = 0; i < count; ++i)
    {
        frame++;

        if (frame > m_EndFrame)
            frame = m_StartFrame;

        // VOID_LOG_ERROR("FRAME: === {0}", frame);

        Request(frame, true);
        AddTask(new CacheNextFrameTask(this));
    }

    // VOID_LOG_INFO("=================================================\n\n\n");

    // // AddTask(new CacheNextFramesTask(count, false, this), 2);
    // for (int i = 0; i < m_Count; ++i)
    // {
    //     // if (m_Threaded)
    //         m_Parent->AddTask(new CacheNextFrameTask(m_Parent));
    //     // else
    //     //     m_Parent->CacheNextFrame();
    // }
}

void ChronoFlux::CachePreviousq()
{
    /* Determine how many frames are used up and can be removed */
    // int count = m_Framenumbers.back() - (m_Player->Frame() + 15);
    int count = Distance(m_Framenumbers.back(), (m_Player->Frame() + 15), m_Duration);

    /* Cannot proceed */
    if (count < 1)
        return;

    int frame = m_LastCached;

    for (int i = 0; i < count; ++i)
    {
        // int frame = m_LastCached - i - 1;
        frame--;

        if (frame < m_StartFrame)
            frame = m_EndFrame;

        Request(frame, true);
        AddTask(new CachePreviousFrameTask(this));
    }

    // AddTask(new CachePreviousFramesTask(count, true, this), 2);
}

v_frame_t ChronoFlux::CurrentFrame() const
{
    if (m_Player)
        return m_Player->Frame();

    return 0;
}

VOID_NAMESPACE_CLOSE
