// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _AUDIO_STREAM_BUFFER_H
#define _AUDIO_STREAM_BUFFER_H

/* STD */
#include <atomic>
#include <future>
#include <vector>

/* Internal */
#include "Definition.h"
#include "Audio.h"
#include "VoidObjects/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

class AudioStreamBuffer
{
public:
    AudioStreamBuffer() = default;
    ~AudioStreamBuffer();

    void SetMedia(const SharedMediaClip& media);
    SharedFrameAudio At(v_frame_t frame) const { return m_Buffer[frame - m_StartFrame]; }

private:
    std::vector<SharedFrameAudio> m_Buffer;
    SharedMediaClip m_Media;
    v_frame_t m_StartFrame;    
    std::future<void> m_Worker;
    std::atomic_bool m_StopToken = false;

private:
    void Cache();
    void StopCaching();
};

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_STREAM_BUFFER_H
