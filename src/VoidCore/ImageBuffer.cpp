// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ImageBuffer.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

FrameBuffer::FrameBuffer()
    : m_MaxMemory(1 * 1024 * 1024 * 1024) // 1 GB by default
    , m_FrameSize(0)
{
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
        return true;

    if (m_FrameSize > (m_MaxMemory - m_UsedMemory))
    {
        if (evict)
        {
            Evict(m_Framenumbers.front());
            m_Framenumbers.pop_front();
            return true;
        }

        /* Can't cache as it might overflow the allowed memory usage */
        return false;
    }

    /* Good to be cached */
    return true;
}

void FrameBuffer::Cache(v_frame_t frame)
{
    VOID_LOG_INFO("Image Buffer Caching Frame {0}", frame);
    VOID_LOG_INFO("Available Memory {0} bytes", (m_MaxMemory - m_UsedMemory));
    Tools::VoidProfiler<std::chrono::duration<double>> p("Frame Cache");

    /* Cache already exists for the frame */
    if (std::find(m_Framenumbers.begin(), m_Framenumbers.end(), frame) != m_Framenumbers.end())
        return;

    if (m_Frames.find(frame) != m_Frames.end())
    {
        Frame* f = m_Frames.at(frame);
        f->Cache();

        m_FrameSize = f->FrameSize();
        m_UsedMemory += m_FrameSize;

        m_Framenumbers.push_back(frame);
        return;
    }

    Frame* f = m_Media->GetFrameRef(frame);
    f->Cache();

    m_FrameSize = f->FrameSize();
    m_UsedMemory += m_FrameSize;

    m_Frames[frame] = f;
    m_Framenumbers.push_back(frame);
}

void FrameBuffer::Evict(v_frame_t frame)
{
    VOID_LOG_INFO("Evicting Frame: {0}", frame);
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
    }
}

VOID_NAMESPACE_CLOSE
