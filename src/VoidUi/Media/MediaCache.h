// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_CACHE_H
#define _VOID_MEDIA_CACHE_H

/* STD */
#include <atomic>
#include <deque>
#include <thread>
#include <memory>
#include <mutex>

/* Qt */
#include <QObject>
#include <QThreadPool>
#include <QRunnable>
#include <QTimer>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/Track.h"

VOID_NAMESPACE_OPEN

/* Forward decl for Player as this needs to know the current playback type and frame */
class Player;

/**
 * ChronoFlux is the Cache Engine for 'VOID'
 * The name itself is an indication of something that flows over time
 * here the flowing entity is Media 'Frames' allowing data to be pre-read (cached)
 * and ready for playback
 */

class VOID_API ChronoFlux : public QObject
{
    Q_OBJECT

public:
    enum class Direction
    {
        Forwards    = -1,
        Backwards   = 1
    };

public:
    explicit ChronoFlux(QObject* parent = nullptr);
    ~ChronoFlux();

    /**
     * Setup the player for which the caching happens
     * this is required to understand what is the current buffer or the current time
     */
    inline void SetActivePlayer(Player* player) { m_Player = player; }

    /**
     * The media to currently look at actively to cache
     */
    void SetMedia(const SharedMediaClip& media);
    // void SetTrack(std::weak_ptr<Track> track);

    inline void SetMaxMemory(unsigned int size_in_gigs) { m_MaxMemory = size_in_gigs * 1024 * 1024 * 1024; }

    /**
     * A Cache process which caches all frames till the memory size allows
     */
    void CacheAvailableFrames();

    void StartPlaybackCache();
    void StopPlaybackCache();

    /**
     * Request caching the next available frame depending on the direction
     * if the memory limit allows based on the average memory usage of the current frame
     * then the request will be approved and true will be returned
     * else false is returned to indicate that the next frame cannot be added to the cache
     *
     * however, the evict flag allows a frame to be cached despite the memory limit
     * as evicting would remove the First or the Last frame depending on the Direction of cache
     * if memory limit does not allow
     */
    bool Request(v_frame_t frame, bool evict = false);

    /**
     * Cache the provided frame for the media, if the frame is already cached nothing happens in terms
     * of memory usage
     */
    void Cache(v_frame_t frame);

    void EnsureCached(v_frame_t frame);

    void CacheAvailable();
    void CacheNextFrame();
    void CachePreviousFrame();

    void CacheNextq();
    void CachePreviousq();
    void CacheNext(int count, bool threaded = false) { AddTask(new CacheNextFramesTask(count, threaded, this)); }
    void CachePrevious(int count, bool threaded = false) { AddTask(new CachePreviousFramesTask(count, threaded, this)); }

private: /* Members */
    Player* m_Player;
    QThreadPool m_ThreadPool;

    /**
     * This is a non-owning pointer to the Media coming in from either the viewer buffer or some other
     * component that might need the media to be cache for some reason
     */
    // std::weak_ptr<MediaClip> m_Media;
    SharedMediaClip m_Media;

    Direction m_CacheDirection;

    /**
     * Bytes representation of the amount of maximum available memory for caching media
     * if the memory is full then the caching is stopped unless a force request (evict = true) is made
     * this makes the last first or frame based on direction be evicted (cleared of any cached data) from memory
     */
    size_t m_MaxMemory;
    size_t m_UsedMemory;

    size_t m_FrameSize;

    std::deque<v_frame_t> m_Framenumbers;

    v_frame_t m_StartFrame;
    v_frame_t m_EndFrame;
    unsigned int m_Duration;

    v_frame_t m_Current;
    v_frame_t m_LastCached;

    std::atomic<bool> m_Cache;

    QTimer m_ForwardsTimer;
    QTimer m_CacheTimer;

    std::mutex m_Mutex;

private: /* Methods */
    inline long AvailableMemory() const { return m_MaxMemory - m_UsedMemory; }
    inline v_frame_t MinFrame() const { return m_Framenumbers.front(); }
    v_frame_t CurrentFrame() const;

    void Evict(v_frame_t frame);
    void EvictFront();
    void EvictBack();

    void Update();

    inline void AddTask(QRunnable* runnable, int priority = 0) { m_ThreadPool.start(runnable, priority); }

    v_frame_t GetNextFrame();
    v_frame_t GetPreviousFrame();

    inline int Distance(int from, int to, int max) { return ( (from - to) + max) % max; }

private: /* Task Classes */
    class CacheFrameTask : public QRunnable
    {
    public:
        CacheFrameTask(v_frame_t frame, ChronoFlux* parent)
            : m_Frame(frame)
            , m_Parent(parent)
        {
        }

        void run()
        {
            m_Parent->Cache(m_Frame);
        }

    private:
        v_frame_t m_Frame;
        ChronoFlux* m_Parent;
    };

    class CacheNextFrameTask : public QRunnable
    {
    public:
        explicit CacheNextFrameTask(ChronoFlux* parent)
            : m_Parent(parent) {}

        void run() override
        {
            m_Parent->CacheNextFrame();
        }

    private:
        ChronoFlux* m_Parent;
    };

    class CacheNextFramesTask : public QRunnable
    {
    public:
        CacheNextFramesTask(int count, bool threaded, ChronoFlux* parent)
            : m_Count(count), m_Threaded(threaded), m_Parent(parent) {}

        void run() override
        {
            for (int i = 0; i < m_Count; ++i)
            {
                if (m_Threaded)
                    m_Parent->AddTask(new CacheNextFrameTask(m_Parent));
                else
                    m_Parent->CacheNextFrame();
            }
        }

    private:
        int m_Count;
        bool m_Threaded;
        ChronoFlux* m_Parent;
    };

    class CachePreviousFrameTask : public QRunnable
    {
    public:
        explicit CachePreviousFrameTask(ChronoFlux* parent)
            : m_Parent(parent) {}

        void run() override
        {
            m_Parent->CachePreviousFrame();
        }

    private:
        ChronoFlux* m_Parent;
    };

    class CachePreviousFramesTask : public QRunnable
    {
    public:
        CachePreviousFramesTask(int count, bool threaded, ChronoFlux* parent)
            : m_Count(count), m_Threaded(threaded), m_Parent(parent) {}

        void run() override
        {
            for (int i = 0; i < m_Count; ++i)
            {
                if (m_Threaded)
                    m_Parent->AddTask(new CachePreviousFrameTask(m_Parent));
                else
                    m_Parent->CachePreviousFrame();
            }
        }

    private:
        int m_Count;
        bool m_Threaded;
        ChronoFlux* m_Parent;
    };

    

    class CacheAvailableFramesTask : public QRunnable
    {
    public:
        CacheAvailableFramesTask(ChronoFlux* parent)
            : m_Parent(parent)
        {
        }

        void run()
        {
            for(size_t frame = m_Parent->m_StartFrame; frame <= m_Parent->m_EndFrame; ++frame)
            {
                if (m_Parent->Request(frame, false))
                {
                    m_Parent->AddTask(new CacheFrameTask(frame, m_Parent));
                }
            }
        }

    private:
        ChronoFlux* m_Parent;
    };

    class CachePlayingFramesTask : public QRunnable
    {
    public:
        CachePlayingFramesTask(ChronoFlux* parent)
            : m_Parent(parent)
        {
        }

        void run()
        {
            bool evict;

            v_frame_t frame = m_Parent->m_StartFrame;

            while (m_Parent->m_Cache)
            {
                VOID_LOG_INFO("P C: {0}--{1}", frame, m_Parent->CurrentFrame());

                evict = (m_Parent->CurrentFrame() - m_Parent->MinFrame()) > 2;
                VOID_LOG_INFO("P C b: {0}--{1}", evict, m_Parent->CurrentFrame());

                if (m_Parent->Request(frame, evict))
                {
                    m_Parent->AddTask(new CacheFrameTask(frame, m_Parent));
                    frame++;
                }

                // if (std::find(m_Parent->m_Framenumbers.begin(), m_Parent->m_Framenumbers.end(), frame) == m_Parent->m_Framenumbers.end())
                // {
                //     evict = (m_Parent->CurrentFrame() - m_Parent->MinFrame()) > 2;
                //     VOID_LOG_INFO("P C b: {0}--{1}", evict, m_Parent->CurrentFrame());

                //     if (m_Parent->Request(frame, evict))
                //     {
                //         m_Parent->AddTask(new CacheFrameTask(frame, m_Parent));
                //         frame++;
                //     }
                // }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                // frame++;

                if (frame > m_Parent->m_EndFrame)
                {
                    /* Restart */
                    frame = m_Parent->m_StartFrame;
                }
            }
            // for(size_t frame = m_Parent->m_StartFrame; frame <= m_Parent->m_EndFrame; ++frame)
            // {
            //     if (m_Parent->Request(frame, false))
            //     {
            //         m_Parent->AddTask(new CacheFrameTask(frame, m_Parent));
            //     }
            // }

        }

    private:
        ChronoFlux* m_Parent;
    };
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_CACHE_H
