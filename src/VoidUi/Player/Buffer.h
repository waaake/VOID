// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PLAYER_BUFFER_H
#define _PLAYER_BUFFER_H

/* STD */
#include <deque>
#include <mutex>
#include <unordered_set>

/* Qt */
#include <QObject>
#include <QRunnable>
#include <QThreadPool>
#include <QTimer>

/* Internal */
#include "Definition.h"
#include "VoidAudio/Core/AudioStream.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class Timeline;

enum class BufferDirection : short
{
    FORWARDS,
    BACKWARDS,
    STOPPED
};

class PlayBuffer : public QObject
{
    Q_OBJECT

    class CacheNextFrameTask : public QRunnable
    {
    public:
        explicit CacheNextFrameTask(PlayBuffer* parent) : m_Parent(parent) {}
        inline void run() override { m_Parent->CacheNextFrame(); }

    private:
        PlayBuffer* m_Parent;
    };

    class CachePreviousFrameTask : public QRunnable
    {
    public:
        explicit CachePreviousFrameTask(PlayBuffer* parent) : m_Parent(parent) {}
        inline void run() override { m_Parent->CachePreviousFrame(); }

    private:
        PlayBuffer* m_Parent;
    };

public:
    PlayBuffer(QObject* parent = nullptr);
    ~PlayBuffer();
    
    void SetTimeline(Timeline* timeline) { m_Timeline = timeline; }

    void SetMedia(const SharedMediaClip& media);
    void SetTrack(const SharedPlaybackTrack& track);
    void SetSequence(const SharedPlaybackSequence& sequence);

    SharedPixels Image(v_frame_t frame);

    inline void SetMaxMemory(unsigned long long gigs) { m_MaxMemory = gigs * 1024 * 1024 * 1024; }
    inline void SetMaxThreads(unsigned int count) { m_ThreadPool.setMaxThreadCount(count); }

    void Start(const BufferDirection& direction = BufferDirection::FORWARDS);
    void Pause();
    void Stop();
    void Disable();
    void Resume();

    void Clear();
    void Recache();

private: /* Methods */
    /**
     * @brief Request buffering the next available frame depending on the direction
     * if the memory limit allows based on the average memory usage of the current frame
     * then the request will be approved and true will be returned
     * else false is returned to indicate that the next frame cannot be added to the cache
     *
     * however, the evict flag allows a frame to be cached despite the memory limit
     * as evicting would remove the First or the Last frame depending on the Direction of cache
     * if memory limit does not allow
     */
    bool Request(v_frame_t frame, bool evict = false);
    void EvictFront();
    void EvictBack();

    inline std::size_t AvailableMemory() { return m_MaxMemory - m_UsedMemory; }

    /**
     * Cache the provided frame for the media, if the frame is already cached nothing happens in terms
     * of memory usage
     */
    // void Load(v_frame_t frame);
    void Buffer(v_frame_t frame);
    void EnsureBuffered(v_frame_t frame);
    void BufferMaxAvailable();

    v_frame_t GetNextFrame();
    v_frame_t GetPreviousFrame();

    void CacheNextFrame();
    void CachePreviousFrame();

    void CacheNext();
    void CachePrevious();

    void MarkBuffered(v_frame_t frame);
    void UnmarkBuffered(v_frame_t frame);

    void PushSamples(const SharedMediaClip& media, v_frame_t requested);

    inline bool Buffered(v_frame_t frame) const { return m_BufferedSet.find(frame) != m_BufferedSet.end(); }
    inline void AddTask(QRunnable* runnable) { m_ThreadPool.start(runnable); }

private: /* Members */
    Timeline* m_Timeline;

    /**
     * This is a non-owning pointer to the Media coming in from either the viewer buffer or some other
     * component that might need the media to be cache for some reason
     */
    std::weak_ptr<MediaClip> m_Media;

    int m_BackBuffer, m_Duration;
    int m_Current;
    int m_SamplesPerFrame;

    // std::size_t m_Framesize;
    v_frame_t m_Start, m_End, m_Last;

    /**
     * Bytes representation of the amount of maximum available memory for caching media
     * if the memory is full then the caching is stopped unless a force request (evict = true) is made
     * this makes the last first or frame based on direction be evicted (cleared of any cached data) from memory
     */
    std::size_t m_MaxMemory;
    std::size_t m_UsedMemory;
    std::size_t m_Framesize;

    AudioStream m_Stream;

    BufferDirection m_Direction;
    std::deque<v_frame_t> m_Buffered;
    std::unordered_set<v_frame_t> m_BufferedSet;

    std::mutex m_Mutex;
    QThreadPool m_ThreadPool;
};

VOID_NAMESPACE_CLOSE

#endif // _PLAYER_BUFFER_H
