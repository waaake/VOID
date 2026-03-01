// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include <iostream>

/* Internal */
#include "Buffer.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Preferences/Preferences.h"
#include "VoidUi/Timeline/Timeline.h"

VOID_NAMESPACE_OPEN

// std::ostream& operator<<(std::ostream& stream, const std::unordered_set<v_frame_t>& f)
// {
//     stream << "Set::[ ";

//     for (int i = 0; i < f.size(); ++i)
//         stream << i << ", ";
    
//     stream << " ]";
//     return stream;
// }

PlayBuffer::PlayBuffer(QObject* parent)
    : QObject(parent)
    , m_Timeline(nullptr)
    , m_Media()
    , m_BackBuffer(5)
    , m_Duration(0)
    , m_Current(0)
    , m_SamplesPerFrame(2)
    , m_MaxMemory(VoidPreferences::Instance().GetCacheMemory() * 1024 * 1024 * 1024) // 1 GB by default
    , m_UsedMemory(0)
    , m_Framesize(0)
    , m_Direction(BufferDirection::FORWARDS)
{
    m_ThreadPool.setMaxThreadCount(VoidPreferences::Instance().GetCacheThreads());
}

PlayBuffer::~PlayBuffer()
{
}

SharedPixels PlayBuffer::Image(v_frame_t frame)
{
    // if (!m_Media)
    m_Current = frame;
    EnsureBuffered(frame);

    if (SharedMediaClip media = m_Media.lock())
    {
        SharedPixels image = media->Image(frame, false);
        // m_Stream.WriteSamples(image->AudioSamples(), image->AudioBufferSize());
        // PushSamples(media, frame);
        return image;
    }

    return nullptr;
}

void PlayBuffer::PushSamples(const SharedMediaClip& media, v_frame_t requested)
{
    if (requested % m_SamplesPerFrame == 0)
    {
        for (int i = 0; i < m_SamplesPerFrame; ++i)
        {
            if (media->HasFrame(requested + i))
            {
                SharedPixels image = media->Image(requested + i, false);
                m_Stream.WriteSamples(image->AudioSamples(), image->AudioBufferSize());
            }
            VOID_LOG_INFO("Pushing...");
        }
    }
}

void PlayBuffer::SetMedia(const SharedMediaClip& media)
{
    Clear();

    m_Media = media;
    m_Start = media->FirstFrame();
    m_End = media->LastFrame();
    m_Duration = media->Duration();

    // if (media->HasAudio())
    //     m_Stream = std::move(AudioStream(media->Samplerate(), media->AudioChannels()));

    m_Current = m_Start;
    EnsureBuffered(m_Start);
    m_Framesize = media->FrameSize();
    BufferMaxAvailable();
}

void PlayBuffer::SetTrack(const SharedPlaybackTrack& track)
{

}

void PlayBuffer::SetSequence(const SharedPlaybackSequence& sequence)
{

}

void PlayBuffer::Start(const BufferDirection& direction)
{
    m_Direction = direction;
    // m_Stream.Start();
}

void PlayBuffer::Pause()
{

}

void PlayBuffer::Stop()
{
    // m_Stream.Stop();
}

void PlayBuffer::Disable()
{

}

void PlayBuffer::Resume()
{

}

void PlayBuffer::Clear()
{
    if (SharedMediaClip media = m_Media.lock())
        media->ClearCache();

    m_Buffered.clear();
    m_BufferedSet.clear();
    m_UsedMemory = 0;
}

void PlayBuffer::Recache()
{
    
}

bool PlayBuffer::Request(v_frame_t frame, bool evict)
{
    if (!m_Framesize)
    {
        m_Direction == BufferDirection::BACKWARDS ? m_Buffered.push_front(frame) : m_Buffered.push_back(frame);
        m_BufferedSet.insert(frame);
        return true;
    }

    // All the frames have been cached
    if (m_Buffered.size() >= m_Duration)
        return false;

    if (m_Framesize > AvailableMemory())
    {
        if (evict)
        {
            if (m_Direction == BufferDirection::BACKWARDS)
            {
                EvictBack();
                m_Buffered.push_front(frame);
            }
            else 
            {
                EvictFront();
                m_Buffered.push_back(frame);
            }
            m_BufferedSet.insert(frame);
            m_UsedMemory += m_Framesize;
            return true;
        }

        // Cannot grant this request as we do not have enough memory
        return false;
    }

    m_UsedMemory += m_Framesize;
    m_Direction == BufferDirection::BACKWARDS ? m_Buffered.push_front(frame) : m_Buffered.push_back(frame);
    m_BufferedSet.insert(frame);
    return true;
}

void PlayBuffer::EvictFront()
{
    v_frame_t frame = m_Buffered.front();

    if (SharedMediaClip media = m_Media.lock())
        media->UncacheFrame(frame);

    m_UsedMemory -= m_Framesize;
    m_Buffered.pop_front();
    m_BufferedSet.erase(frame);
    UnmarkBuffered(frame);
}

void PlayBuffer::EvictBack()
{
    v_frame_t frame = m_Buffered.back();

    if (SharedMediaClip media = m_Media.lock())
        media->UncacheFrame(frame);

    m_UsedMemory -= m_Framesize;
    m_Buffered.pop_back();
    m_BufferedSet.erase(frame);
    UnmarkBuffered(frame);
}

void PlayBuffer::Buffer(v_frame_t frame)
{
    if (SharedMediaClip media = m_Media.lock())
    {
        if (media->Contains(frame))
        {
            // media->CacheFrame(frame);
            SharedPixels image = media->Image(frame);
            // image->A
            // VOID_LOG_INFO("Buffering frame: {0}", frame);
            MarkBuffered(frame);
        }
    }
}

void PlayBuffer::EnsureBuffered(v_frame_t frame)
{
    if (m_BufferedSet.find(frame) == m_BufferedSet.end())
    {
        VOID_LOG_INFO("Force Caching Frame: {0}", frame);
        // std::cout << m_BufferedSet << "\n";
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Last = frame;
        }

        /* Evict a frame if necessary as this needs to be cached */
        Request(frame, true);
        Buffer(frame);
    }
}

void PlayBuffer::BufferMaxAvailable()
{
    /**
     * This is invoked whenever the media is set/changed on the player buffer
     * so the usual direction is Forwards, unless we were going backwards
     */
    if (m_Direction == BufferDirection::BACKWARDS)
    {
        for (v_frame_t frame = m_End; frame >= m_Start; --frame)
        {
            if (Buffered(frame))
                continue;

            if (!Request(frame, false))
                return;

            AddTask(new CachePreviousFrameTask(this));
        }
    }
    else
    {
        for (v_frame_t frame = m_Start; frame <= m_End; ++frame)
        {
            if (Buffered(frame))
                continue;

            if (!Request(frame, false))
                return;

            AddTask(new CacheNextFrameTask(this));
        }
    }
}

v_frame_t PlayBuffer::GetNextFrame()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (m_Last >= m_End)
        m_Last = m_Start;
    else
        ++m_Last;

    return m_Last;
}

v_frame_t PlayBuffer::GetPreviousFrame()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (m_Last <= m_Start)
        m_Last = m_End;
    else
        --m_Last;
    
    return m_Last;
}

void PlayBuffer::MarkBuffered(v_frame_t frame)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Timeline->AddCacheFrame(frame);
}

void PlayBuffer::UnmarkBuffered(v_frame_t frame)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Timeline->RemoveCachedFrame(frame);
}

void PlayBuffer::CacheNextFrame()
{
    Buffer(GetNextFrame());
}

void PlayBuffer::CachePreviousFrame()
{
    Buffer(GetPreviousFrame());
}

void PlayBuffer::CacheNext()
{
    v_frame_t frame = m_Last;
    
    /* Determine how many frames are used up and can be removed */
    while (!m_Buffered.empty())
    {
        /* Ensure the cached frame is just between the current frame and the back buffer */
        if (m_Current - m_BackBuffer < m_Buffered.front() && m_Buffered.front() <= m_Current)
            break;

        frame++;

        if (frame > m_End)
            frame = m_Start;

        Request(frame, true);
        AddTask(new CacheNextFrameTask(this));
    }
}

void PlayBuffer::CachePrevious()
{
    v_frame_t frame = m_Last;

    /* Determine how many frames are used up and can be removed */
    while (!m_Buffered.empty())
    {
        /* Ensure the cached frame is just between the current frame and the back buffer */
        if (m_Current + m_BackBuffer > m_Buffered.back() && m_Buffered.back() >= m_Current)
            break;

        frame--;

        if (frame < m_Start)
            frame = m_End;

        Request(frame, true);
        AddTask(new CachePreviousFrameTask(this));
    }
}

VOID_NAMESPACE_CLOSE
