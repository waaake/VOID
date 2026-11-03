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

struct BufferChunk
{
    v_frame_t start, end;
    std::vector<SharedFrameAudio> buffer;

    bool Contains(v_frame_t frame) const { return frame >= start && frame <= end; }
    void Reset() { start = -1; end = -1; }
    SharedFrameAudio operator[](v_frame_t frame) const { return buffer[frame - start]; }
};

class AudioStreamBuffer
{
public:
    AudioStreamBuffer();
    ~AudioStreamBuffer();

    void SetMedia(const SharedMediaClip& media);
    // SharedFrameAudio At(v_frame_t frame) const { return m_Media ? m_Buffer[frame - m_StartFrame] : nullptr; }
    SharedFrameAudio At(v_frame_t frame);

private:
    static inline constexpr int s_NumChunks = 2;
    std::vector<SharedFrameAudio> m_Buffer;
    BufferChunk m_Chunks[s_NumChunks];
    BufferChunk m_Temp;
    SharedMediaClip m_Media;
    v_frame_t m_StartFrame;
    int m_ActiveChunk;
    std::future<void> m_Worker;
    std::atomic_bool m_StopToken;

private:
    void Cache();
    // void CacheChunk(v_frame_t startframe, int startidx, std::size_t size);
    // void CacheChunk(v_frame_t startframe, std::size_t size, BufferChunk& chunk);
    void CacheChunk(v_frame_t startframe, int index);
    void CacheTemp(v_frame_t startframe);
    void StopCaching();
};

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_STREAM_BUFFER_H
