// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "AudioStreamBuffer.h"
#include "VoidBridge/Engine.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

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
    
        m_Worker = std::async(std::launch::async, &AudioStreamBuffer::Cache, this);
    }
    else
    {
        /// Clear the underlying media as we now don't have any active media to read audio from
        /// this also results in no results when audio for a frame is requested
        m_Media.reset();
        VOID_LOG_INFO("No Audio set.");
    }
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

void AudioStreamBuffer::StopCaching()
{
    m_StopToken = true;
    if (m_Worker.valid())
        m_Worker.get();
}

VOID_NAMESPACE_CLOSE
