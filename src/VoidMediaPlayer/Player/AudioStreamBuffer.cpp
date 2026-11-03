// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <functional>

/* Internal */
#include "AudioStreamBuffer.h"
#include "VoidBridge/Engine.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

static constexpr unsigned int s_MAX_CHUNK_SIZE = 50;
static constexpr unsigned int s_MAX_TEMP_CHUNK_SIZE = 10;
static constexpr unsigned int s_MAX_BUFFER_SIZE = 100;

AudioStreamBuffer::AudioStreamBuffer()
    : m_Media(nullptr)
    , m_ActiveChunk(0)
    , m_StopToken(false)
{
    m_Buffer.resize(s_MAX_BUFFER_SIZE);

    m_Temp.buffer.resize(s_MAX_TEMP_CHUNK_SIZE);
    m_Chunks[0].buffer.resize(s_MAX_CHUNK_SIZE);
    m_Chunks[1].buffer.resize(s_MAX_CHUNK_SIZE);
}

AudioStreamBuffer::~AudioStreamBuffer()
{
    StopCaching();
}

void AudioStreamBuffer::SetMedia(const SharedMediaClip& media)
{
    StopCaching();

    if (media->HasAudio() && _EngineBridge.AudioEnabled())
    {
        m_Media = media;
        m_StartFrame = m_Media->FirstFrame();
    
        m_Buffer.resize(m_Media->Duration());
        m_StopToken = false;
    
        // m_Worker = std::async(std::launch::async, &AudioStreamBuffer::Cache, this);
        // m_Worker = std::async(std::launch::async, &AudioStreamBuffer::CacheChunk, this, m_Media->FirstFrame(), 0, s_MAX_BUFFER_SIZE / 2);
        m_ActiveChunk = 0;
        m_Worker = std::async(std::launch::async, &AudioStreamBuffer::CacheChunk, this, m_Media->FirstFrame(), m_ActiveChunk);
    }
    else
    {
        /// Clear the underlying media as we now don't have any active media to read audio from
        /// this also results in no results when audio for a frame is requested
        m_Media.reset();
        VOID_LOG_INFO("No Audio set.");
    }
}

SharedFrameAudio AudioStreamBuffer::At(v_frame_t frame)
{
    if (m_Media)
    {
        // constexpr unsigned int half = s_MAX_BUFFER_SIZE / 2;
        // constexpr unsigned int firstpoint = half / 2;
        // constexpr unsigned int secondpoint = half + firstpoint;

        // /// This is our signal to start caching the next chunk of frames
        // if (frame == firstpoint)
        // {
        //     m_Worker = std::async(std::launch::async, &AudioStreamBuffer::CacheChunk, this, m_Media->FirstFrame() + half, half, half);
        // }
        // else if (frame == secondpoint)
        // {
        //     v_frame_t last = m_Media->FirstFrame() + s_MAX_BUFFER_SIZE;
        //     if (last < half / 2)
        //         m_Worker = std::async(std::launch::async, &AudioStreamBuffer::CacheChunk, this, last, half, half);
        //     else
        //         m_Worker = std::async(std::launch::async, &AudioStreamBuffer::CacheChunk, this, last, 0, half);
        // }
        
        // int index = frame - m_StartFrame;
        // if (index >= s_MAX_BUFFER_SIZE)
        // {
        //     // change m_StartFrame here ---
        //     index = frame - m_StartFrame;
        // }

        // return m_Buffer[index];

        /// Find in the active chunk, if we can find in the active chunk
        const BufferChunk& chunk = m_Chunks[m_ActiveChunk];
        if (chunk.Contains(frame))
            return chunk[frame];

        m_ActiveChunk = (m_ActiveChunk + 1) % s_NumChunks;
        BufferChunk& next = m_Chunks[m_ActiveChunk];
        if (next.Contains(frame))
            return next[frame];

        /// Here we are at a point where the requested frame doesn't exist on any of the cache chunks, cache miss
        /// in order to return the relevant frame quickly we only need to buffer the requested with a few more in the direction
        CacheTemp(frame);
        // And also start the cache for the frames now
        m_Worker = std::async(std::launch::async, &AudioStreamBuffer::CacheChunk, this, frame, m_ActiveChunk);
        return m_Temp[frame];
    }

    return nullptr;
}

void AudioStreamBuffer::Cache()
{
    for (v_frame_t i = m_StartFrame; i <= m_Media->LastFrame(); ++i)
    {
        /// If we are loading something else or existing, stop the cache process
        if (m_StopToken.load())
            return;

        SharedFrameAudio& buffer = m_Buffer[i - m_StartFrame];
        if (!buffer)
            buffer = std::make_shared<FrameAudio>();

        buffer->Clear();
        m_Media->Audio(i, buffer);
        VOID_LOG_INFO("Buffering audio for frame... {}", i);
    }
}

// void AudioStreamBuffer::CacheChunk(v_frame_t startframe, int startidx, std::size_t size)
// {
//     VOID_LOG_INFO("CacheChunk: {} {} {}", startframe, startidx, size);
//     v_frame_t lastframe = m_Media->LastFrame();
//     int last = std::min((v_frame_t)(startframe + size), lastframe);
//     // int lastidx = std::min(int(startidx + size), (int)(m_Media->LastFrame() - startframe));
//     std::size_t s = std::min(size, (std::size_t)((m_Media->LastFrame() - startframe) + 1));

//     // Media doesn't have enough frames to buffer anymore
//     if (last < startframe)
//         return;

//     for (int i = startidx; i < startidx + s; ++i)
//     {
//         if (m_StopToken.load())
//             return;

//         SharedFrameAudio& buffer = m_Buffer[i];
//         if (!buffer)
//             buffer = std::make_shared<FrameAudio>();

//         buffer->Clear();
//         v_frame_t frame = startframe + i - startidx;
//         m_Media->Audio(frame, buffer);
//         VOID_LOG_INFO("Buffering audio for frame... {}", frame);
//     }
// }

// void AudioStreamBuffer::CacheChunk(v_frame_t startframe, std::size_t size, BufferChunk& chunk)
// {
//     v_frame_t lastframe = m_Media->LastFrame();
//     int last = std::min((v_frame_t)(startframe + size), lastframe);
//     // int lastidx = std::min(int(startidx + size), (int)(m_Media->LastFrame() - startframe));
//     std::size_t s = std::min((std::size_t)size, (std::size_t)((m_Media->LastFrame() - startframe) + 1));

//     // Media doesn't have enough frames to buffer anymore
//     if (last < startframe)
//         return chunk.Reset();

//     chunk.start = startframe;
//     chunk.end = startframe + s - 1;

//     VOID_LOG_INFO("Buffer size: {0}", chunk.buffer.size());

//     // chunk.buffer.resize(size);
//     for (int i = 0; i < s; ++i)
//     {
//         if (m_StopToken.load())
//             return;

//         SharedFrameAudio& buffer = chunk.buffer[i];
//         if (!buffer)
//             buffer = std::make_shared<FrameAudio>();

//         buffer->Clear();
//         v_frame_t frame = startframe + i;
//         m_Media->Audio(frame, buffer);
//         VOID_LOG_INFO("Buffering audio for frame... {}", frame);
//     }

//     VOID_LOG_INFO("Chunk Contains: {0} - {1}", chunk.start, chunk.end);
// }

void AudioStreamBuffer::CacheChunk(v_frame_t startframe, int index)
{
    VOID_LOG_INFO("RRR: {}", index);
    BufferChunk& chunk = m_Chunks[index];

    v_frame_t lastframe = m_Media->LastFrame();
    int last = std::min((v_frame_t)(startframe + s_MAX_CHUNK_SIZE), lastframe);
    // int lastidx = std::min(int(startidx + size), (int)(m_Media->LastFrame() - startframe));
    std::size_t s = std::min((std::size_t)s_MAX_CHUNK_SIZE, (std::size_t)((m_Media->LastFrame() - startframe) + 1));

    // Media doesn't have enough frames to buffer anymore
    if (last < startframe)
        return chunk.Reset();

    chunk.start = startframe;
    chunk.end = startframe + s - 1;

    VOID_LOG_INFO("Buffer size: {0}", chunk.buffer.size());

    // chunk.buffer.resize(size);
    for (int i = 0; i < s; ++i)
    {
        if (m_StopToken.load())
            return;

        SharedFrameAudio& buffer = chunk.buffer[i];
        if (!buffer)
            buffer = std::make_shared<FrameAudio>();

        buffer->Clear();
        v_frame_t frame = startframe + i;
        m_Media->Audio(frame, buffer);
        VOID_LOG_INFO("Buffering audio for frame... {}", frame);
    }

    VOID_LOG_INFO("Chunk Contains: {0} - {1}", chunk.start, chunk.end);
}

void AudioStreamBuffer::CacheTemp(v_frame_t startframe)
{
    v_frame_t lastframe = m_Media->LastFrame();
    int last = std::min((v_frame_t)(startframe + s_MAX_TEMP_CHUNK_SIZE), lastframe);
    // int lastidx = std::min(int(startidx + size), (int)(m_Media->LastFrame() - startframe));
    std::size_t s = std::min((std::size_t)s_MAX_TEMP_CHUNK_SIZE, (std::size_t)((m_Media->LastFrame() - startframe) + 1));

    // Media doesn't have enough frames to buffer anymore
    if (last < startframe)
        return m_Temp.Reset();

    m_Temp.start = startframe;
    m_Temp.end = startframe + s - 1;

    VOID_LOG_INFO("Buffer size: {0}", m_Temp.buffer.size());

    // chunk.buffer.resize(size);
    for (int i = 0; i < s; ++i)
    {
        if (m_StopToken.load())
            return;

        SharedFrameAudio& buffer = m_Temp.buffer[i];
        if (!buffer)
            buffer = std::make_shared<FrameAudio>();

        buffer->Clear();
        v_frame_t frame = startframe + i;
        m_Media->Audio(frame, buffer);
        VOID_LOG_INFO("Buffering audio for frame... {}", frame);
    }

    VOID_LOG_INFO("Chunk Contains: {0} - {1}", m_Temp.start, m_Temp.end);
}

void AudioStreamBuffer::StopCaching()
{
    m_StopToken = true;
    if (m_Worker.valid())
        m_Worker.get();
}

VOID_NAMESPACE_CLOSE
