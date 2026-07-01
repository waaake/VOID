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

/// Tasks

class CacheNextFrameTask : public QRunnable
{
public:
    CacheNextFrameTask(ViewerBuffer* parent) : m_Parent(parent) {}
    inline void run() override { m_Parent->CacheNextFrame(); }

private:
    ViewerBuffer* m_Parent;
};

class CachePreviousFrameTask : public QRunnable
{
public:
    CachePreviousFrameTask(ViewerBuffer* parent) : m_Parent(parent) {}
    inline void run() override { m_Parent->CachePreviousFrame(); }

private:
    ViewerBuffer* m_Parent;
};

/// ViewerBuffer

ViewerBuffer::ViewerBuffer(QObject* parent)
    : PlayerBuffer(parent)
    , m_Name("Viewer")
    , m_Color(130, 110, 190)    // Purple
    , m_Player(nullptr)
    , m_MaxMemory(VoidPreferences::Instance().GetCacheMemory() * 1024 * 1024 * 1024) // 1 GB by default
    , m_Framesize(1)
    , m_Capacity(10)
    , m_LastCached(0)
    , m_State(PlayState::Forwards)
    , m_BackBuffer(3)
    , m_Active(false)
{
    m_ThreadPool.setMaxThreadCount(VoidPreferences::Instance().GetCacheThreads());
    connect(&m_CacheTimer, &QTimer::timeout, this, &ViewerBuffer::Update, Qt::DirectConnection);
    connect(&VoidPreferences::Instance(), &VoidPreferences::updated, this, &ViewerBuffer::SettingsUpdated);
    connect(this, &PlayerBuffer::mediaAboutToBeUpdated, this, &ViewerBuffer::Refresh, Qt::DirectConnection);
    connect(this, &PlayerBuffer::mediaUpdated, this, &ViewerBuffer::ResetMedia);
    connect(this, &PlayerBuffer::rangeUpdated, this, [this](v_frame_t start, v_frame_t end) -> void
    {
        m_BackBuffer = std::min(10, std::max(3, static_cast<int>(((m_Endframe - m_Startframe) + 1) * 0.02)));
    }, Qt::DirectConnection);
}

ViewerBuffer::~ViewerBuffer()
{
}

void ViewerBuffer::SetColor(const QColor& color)
{
    /* Update the Buffer Color */
    m_Color = color;

    /* Update entities with the color change */
    m_Clip->SetColor(color);
    m_Track->SetColor(color);
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

void ViewerBuffer::Clear()
{
    ClearCache();
    ResetMedia();
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

bool ViewerBuffer::Completed() const
{
    /**
     * The cache/buffer is said to be completed if the internal frame store has the start
     * and the end frames present and the overall duration is same or exceeds the duration of playback
     *
     */
    return m_Numbers.size() >= InternalDuration() && Buffered(InternalStartframe()) && Buffered(InternalEndframe());
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

void ViewerBuffer::ResetMedia()
{
    EnsureCached(m_Startframe);
    CacheAvailable();
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
