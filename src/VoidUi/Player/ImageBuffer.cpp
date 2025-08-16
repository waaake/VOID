// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ImageBuffer.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"
#include "VoidUi/PlayerWidget.h"

VOID_NAMESPACE_OPEN

FrameBuffer::FrameBuffer(Player* player, QObject* parent)
    : QThreadPool(parent)
    , m_MaxMemory(1 * 1024 * 1024 * 1024) // 1 GB by default
    , m_UsedMemory(0)
    , m_FrameSize(0)
    , m_Player(player)
{
    VOID_LOG_INFO("Max Memory: {0}", m_MaxMemory);
    setMaxThreadCount(5);
}

FrameBuffer::~FrameBuffer()
{
}

bool FrameBuffer::Request(bool evict)
{
    /**
     * Size isn't yet set so we can definitely go for caching the first frame
     * well, unless the first frame itself is more than the max available memory
     * (highly unlikely unless we're on 256 MB allocation)
     */
    if (!m_FrameSize)
    {
        m_UsedMemory += m_FrameSize;
        return true;
    }

    if (m_FrameSize > (m_MaxMemory - m_UsedMemory))
    {
        if (evict)
        {
            Evict(m_Framenumbers.front());
            m_Framenumbers.pop_front();

            m_UsedMemory += m_FrameSize;
            return true;
        }

        /* Can't cache as it might overflow the allowed memory usage */
        return false;
    }

    /* Good to be cached */
    m_UsedMemory += m_FrameSize;
    return true;
}

void FrameBuffer::Cache(v_frame_t frame)
{
    // VOID_LOG_INFO("Image Buffer Caching Frame {0}", frame);
    // VOID_LOG_INFO("Available Memory {0} bytes", (m_MaxMemory - m_UsedMemory));
    // Tools::VoidProfiler<std::chrono::duration<double>> p("Frame Cache");
    // VOID_LOG_INFO("Used Memory {0}", m_UsedMemory);
    // VOID_LOG_INFO("Frame Size {0}", m_FrameSize);

    /* Cache already exists for the frame */
    if (std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame) != m_Framenumbers.end())
        return;

    if (m_Frames.find(frame) != m_Frames.end())
    {
        Frame* f = m_Frames.at(frame);
        f->Cache();

        m_FrameSize = f->FrameSize();
        // m_UsedMemory += m_FrameSize;

        m_Framenumbers.push_back(frame);

        m_Player->AddCacheFrame(frame);
        return;
    }

    Frame* f = m_Media->GetFrameRef(frame);
    f->Cache();

    m_FrameSize = f->FrameSize();
    // m_UsedMemory += m_FrameSize;

    m_Frames[frame] = f;
    m_Framenumbers.push_back(frame);

    m_Player->AddCacheFrame(frame);
}

void FrameBuffer::Evict(v_frame_t frame)
{
    // VOID_LOG_INFO("Evicting Frame: {0}", frame);
    /**
     * Since the map is just holding a reference to the frame
     * we don't need to get rid of the frame reference itself, just clear it's underlying
     * data and we can have our free memory back
     */
    if (m_Frames.find(frame) != m_Frames.end())
    {
        Frame* f = m_Frames.at(frame);

        m_UsedMemory -= f->FrameSize();
        f->ClearCache();

        m_Player->RemoveCachedFrame(frame);
    }
}

v_frame_t FrameBuffer::CurrentFrame() const
{
    return m_Player->Frame();
}

void FrameBuffer::CacheAvailableFrames()
{
    if (m_Media->MediaType() == Media::Type::MOVIE)
    {
        // VOID_LOG_INFO("Starting available Frame task...");
        start(new CacheAvailableMovieFramesTask(this));
    }
    else
    {
        // VOID_LOG_INFO("Starting available Frame task...");
        start(new CacheAvailableFramesTask(this));
    }
}

void FrameBuffer::EnsureCached(v_frame_t frame)
{
    if (std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame) == m_Framenumbers.end())
    {
        Request(true);
        Cache(frame);
    }
}

void FrameBuffer::SetMedia(Media* media)
{
    Clear();
    m_Media = media;

    /**
     * Ensure that the first frame is cached
     * This helps with the understanding of the frame size of the media
     */
    EnsureCached(m_Media->FirstFrame());
}

void FrameBuffer::Clear()
{
    for (v_frame_t frame : m_Framenumbers)
    {
        m_Frames.at(frame)->ClearCache();
    }

    m_Frames.clear();
    m_Framenumbers.clear();
}

/* Cache Available Frames Task {{{ */

CacheAvailableFramesTask::CacheAvailableFramesTask(FrameBuffer* framebuffer)
    : QRunnable()
    , m_FrameBuffer(framebuffer)
{
}

void CacheAvailableFramesTask::run()
{
    for (v_frame_t frame = m_FrameBuffer->StartFrame(); frame <= m_FrameBuffer->EndFrame(); ++frame)
    {
        // bool evict = (m_FrameBuffer->MinFrame() - m_FrameBuffer->CurrentFrame()) > 2;
        if (!m_FrameBuffer->Request())
        {
            VOID_LOG_INFO("Stopping");
            return;
        }

        // m_FrameBuffer->Cache(frame);
        // m_FrameBuffer->
        m_FrameBuffer->start(new CacheFramesTask(m_FrameBuffer, frame));
    }
}

/* }}} */

/* Cache Available Movie Frames Task {{{ */

CacheAvailableMovieFramesTask::CacheAvailableMovieFramesTask(FrameBuffer* framebuffer)
    : QRunnable()
    , m_FrameBuffer(framebuffer)
{
}

void CacheAvailableMovieFramesTask::run()
{
    for (v_frame_t frame = m_FrameBuffer->StartFrame(); frame <= m_FrameBuffer->EndFrame(); ++frame)
    {
        if (!m_FrameBuffer->Request())
        {
            VOID_LOG_INFO("Stopping");
            return;
        }
        m_FrameBuffer->Cache(frame);
    }
}

/* }}} */

/* Cache Frame Task {{{ */

CacheFramesTask::CacheFramesTask(FrameBuffer* framebuffer, v_frame_t frame)
    : QRunnable()
    , m_FrameBuffer(framebuffer)
    , m_Frame(frame)
{
}

void CacheFramesTask::run()
{
    m_FrameBuffer->Cache(m_Frame);
}

/* }}} */

VOID_NAMESPACE_CLOSE
