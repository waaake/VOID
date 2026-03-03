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
#include <unordered_set>

/* Qt */
#include <QObject>
#include <QThreadPool>
#include <QRunnable>
#include <QTimer>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/TrackView.h"
#include "VoidObjects/Sequence/SequenceView.h"

VOID_NAMESPACE_OPEN

/* Forward decl for Player as this needs to know the current playback type and frame */
class Player;

/**
 * PlayBuffer is the Cache Engine for 'VOID'
 * Buffering ahead of time for playback readiness
 */
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
        inline void run() override {  m_Parent->CachePreviousFrame(); }

    private:
        PlayBuffer* m_Parent;
    };

public:
    enum class Direction
    {
        None,
		Forwards,
		Backwards
    };

    enum class State
    {
        Paused,
        Enabled,
        Disabled,
    };

    enum class Entity
    {
        Media,
        Track,
        Sequence
    };

public:
    explicit PlayBuffer(QObject* parent = nullptr);
    ~PlayBuffer();

    /**
     * Setup the player for which the caching happens
     * this is required to understand what is the current buffer or the current time
     */
    inline void SetActivePlayer(Player* player) { m_Player = player; }

    /**
     * The media to currently look at actively to cache
     */
    void SetMedia(const SharedMediaClip& media);
    void SetTrack(const SharedPlaybackTrack& track);
    void SetSequence(const SharedPlaybackSequence& sequence);

    inline void SetMaxMemory(unsigned long long gigs) { m_MaxMemory = gigs * 1024 * 1024 * 1024; }
    inline void SetMaxThreads(unsigned int count) { m_ThreadPool.setMaxThreadCount(count); }

    void StartPlaybackCache(const Direction& direction = Direction::Forwards);
    inline void RestartPlaybackCache() { StartPlaybackCache(m_Direction); }
    void StopPlaybackCache();

    /**
     * Pauses the cache till it is manually resumed or another media set as
     * active on the processor
     */
    void PauseCaching();

    /**
     * Stops the Cache Process completely and it is enabled only when it is resumed manually
     */
    void DisableCaching();

    /**
     * Stops the cache process completely
     */
    void StopCaching();
    void ResumeCaching();

    void Recache();
    void ClearCache();

    void EnsureCached(v_frame_t frame);

private: /* Members */
    Player* m_Player;
    QThreadPool m_ThreadPool;

    TrackView* m_TrackView;
    SequenceView* m_SequenceView;

    /**
     * This is a non-owning pointer to the Media coming in from either the viewer buffer or some other
     * component that might need the media to be cache for some reason
     */
    std::weak_ptr<MediaClip> m_Media;
    std::weak_ptr<PlaybackTrack> m_Track;

    Direction m_Direction;
    State m_State;
    Entity m_CacheEntity;

    /**
     * Bytes representation of the amount of maximum available memory for caching media
     * if the memory is full then the caching is stopped unless a force request (evict = true) is made
     * this makes the last first or frame based on direction be evicted (cleared of any cached data) from memory
     */
    std::size_t m_MaxMemory;
    std::size_t m_UsedMemory;
    std::size_t m_FrameSize;

    v_frame_t m_StartFrame;
    v_frame_t m_EndFrame;
    unsigned int m_Duration;

    v_frame_t m_LastCached;
    /**
     * The amount of frames that can be kept on the opposite side of the current sync direction
     * this is kept to atleast support playback till the cache starts updating in the other direction
     * this is being set to a minimum of 3 frames and upto a max of 2% of the overall length of the media
     */
    int m_BackBuffer;

    QTimer m_CacheTimer;
    std::mutex m_Mutex;

    std::deque<v_frame_t> m_Framenumbers;
    std::unordered_set<v_frame_t> m_Buffered;

private: /* Methods */
    inline std::size_t AvailableMemory() const { return m_MaxMemory - m_UsedMemory; }

    /**
     * Frame Eviction from the cached array
     */
    void EvictFront();
    void EvictBack();

    void ClearTrackView();
    void ClearSequenceView();

    /**
     * Update to refresh the cache to available frames after removing the frames
     * that may no longer be required
     */
    void Update();

    void UpdateRange(v_frame_t start, v_frame_t end);
    inline void AddTask(QRunnable* runnable, int priority = 0) { m_ThreadPool.start(runnable, priority); }
    
    /**
     * A Cache process which caches all frames till the memory size allows
     */
    void CacheAvailable();

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

    void CacheNextFrame();
    void CachePreviousFrame();

    void CacheNext();
    void CachePrevious();

    v_frame_t GetNextFrame();
    v_frame_t GetPreviousFrame();

    inline bool Cached(v_frame_t frame) const { return m_Buffered.find(frame) != m_Buffered.end(); }
    void SettingsUpdated();    
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_CACHE_H
