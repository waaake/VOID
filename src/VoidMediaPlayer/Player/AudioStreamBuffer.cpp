// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "AudioStreamBuffer.h"
#include "VoidBridge/Engine.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

static constexpr unsigned int s_MAX_CHUNK_SIZE = 500;
static constexpr unsigned int s_MAX_TEMP_CHUNK_SIZE = 10;

AudioStreamBuffer::AudioStreamBuffer()
    : m_Media(nullptr)
    , m_ActiveChunk(0)
    , m_StopToken(false)
{
    /// short cache to handle frame quickly during cache misses
    /// TODO: check if we want to directly return for the missed frame rather than caching a few frames here?
    m_Temp.buffer.resize(s_MAX_TEMP_CHUNK_SIZE);
    // Actual chunks to hold frame cache
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
        m_StopToken = false;
    
        m_ActiveChunk = 0;
        m_Worker = std::async(
            std::launch::async,
            &AudioStreamBuffer::CacheChunk,
            this,
            m_Media->FirstFrame(),
            m_ActiveChunk
        );
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
        const v_frame_t start = m_Media->FirstFrame();
        const int diff = m_LastRequested - frame;
        const bool backwards = (diff == 1 || diff == 2);

        m_LastRequested = frame;

        /// Find in the active chunk, if we can find in the active chunk
        const BufferChunk& chunk = m_Chunks[m_ActiveChunk];
        if (chunk.Contains(frame))
            return chunk[frame];

        m_ActiveChunk = (m_ActiveChunk + 1) % s_NumChunks;
        const BufferChunk& next = m_Chunks[m_ActiveChunk];
        if (next.Contains(frame))
            return next[frame];

        /// Here we are at a point where the requested frame doesn't exist on any of the cache chunks, cache miss
        /// in order to return the relevant frame quickly we only need to buffer the requested with a few more
        /// in the direction
        if (m_Temp.Contains(frame))
            return m_Temp[frame];

        CacheTemp(backwards ? std::max((frame - s_MAX_TEMP_CHUNK_SIZE) + 1, start) : frame);
        // And also start the cache for the frames now
        m_Worker = std::async(
            std::launch::async,
            &AudioStreamBuffer::CacheChunk,
            this,
            backwards ? std::max((frame - s_MAX_CHUNK_SIZE) + 1, start) : frame,
            m_ActiveChunk
        );
        return m_Temp[frame];
    }

    return nullptr;
}

void AudioStreamBuffer::CacheChunk(v_frame_t startframe, int index)
{
    BufferChunk& chunk = m_Chunks[index];

    const v_frame_t lastframe = m_Media->LastFrame();
    int last = std::min((v_frame_t)(startframe + s_MAX_CHUNK_SIZE), lastframe);
    std::size_t s = std::min((std::size_t)s_MAX_CHUNK_SIZE, (std::size_t)((lastframe - startframe) + 1));

    // Media doesn't have enough frames to buffer anymore
    if (last < startframe)
        return chunk.Reset();

    chunk.start = startframe;
    chunk.end = startframe + s - 1;

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
    }
}

void AudioStreamBuffer::CacheTemp(v_frame_t startframe)
{
    const v_frame_t lastframe = m_Media->LastFrame();
    const int last = std::min((v_frame_t)(startframe + s_MAX_TEMP_CHUNK_SIZE), lastframe);
    std::size_t s = std::min((std::size_t)s_MAX_TEMP_CHUNK_SIZE, (std::size_t)((lastframe - startframe) + 1));

    // Media doesn't have enough frames to buffer anymore
    if (last < startframe)
        return m_Temp.Reset();

    m_Temp.start = startframe;
    m_Temp.end = startframe + s - 1;

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
    }
}

void AudioStreamBuffer::StopCaching()
{
    m_StopToken = true;
    if (m_Worker.valid())
        m_Worker.get();
}

VOID_NAMESPACE_CLOSE
