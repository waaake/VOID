// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_VIEWER_BUFFER_H
#define _VOID_VIEWER_BUFFER_H

/* STD */
#include <atomic>
#include <deque>
#include <mutex>
#include <unordered_set>

/* Qt */
#include <QColor>
#include <QObject>
#include <QRunnable>
#include <QThreadPool>
#include <QTimer>

/* Internal */
#include "Definition.h"
#include "Components.h"
#include "PlayerBuffer.h"

VOID_NAMESPACE_OPEN

/* Forward decl for Player as this needs to know the current playback type and frame */
class Player;
class CacheNextFrameTask;
class CachePreviousFrameTask;

class VOID_API ViewerBuffer : public PlayerBuffer
{
    Q_OBJECT

public: /* Enums */
    enum class PlayState
    {
        Paused,
        Forwards,
        Backwards,
        Enabled,
        Disabled
    };

public:
    explicit ViewerBuffer(QObject* parent = nullptr);
    ~ViewerBuffer();

    /// Attribs

    [[nodiscard]] inline bool Active() const { return m_Active; }
    inline std::string Name() const { return m_Name; }
    inline QColor Color() const { return m_Color; }

    /// Set Attribs

    inline void SetMaxThreads(unsigned int count) { m_ThreadPool.setMaxThreadCount(count); }
    void SetFramesize(std::size_t size);
    void SetMaxMemory(std::size_t gigabytes);

    inline void SetActive(const bool active) { m_Active = active; emit updated(); }
    inline void SetName(const std::string& name) { m_Name = name; }
    void SetColor(const QColor& color);

    /// Set Components

    inline void SetActivePlayer(Player* player) { m_Player = player; }

    // Cache/Buffer

    void StartPlaybackCache(const PlayState& state = PlayState::Forwards);
    inline void RestartPlaybackCache() { StartPlaybackCache(m_State); }
    void StopPlaybackCache();
    void PauseCaching();
    void DisableCaching();
    void StopCaching();
    void ResumeCaching();
    void Recache();
    void ClearCache();

    void Refresh();
    void Clear();

    // ImageData
    const FloatImage Image(const v_frame_t frame);

private: /* Members */
    std::deque<v_frame_t> m_Numbers;
    std::unordered_set<v_frame_t> m_Buffered;

    // Attribs
    std::string m_Name;
    QTimer m_CacheTimer;
    QColor m_Color;
    Player* m_Player;

    std::size_t m_MaxMemory;
    std::atomic<std::size_t> m_Framesize;
    std::atomic<unsigned int> m_Capacity;
    v_frame_t m_LastCached;

    PlayState m_State;
    QThreadPool m_ThreadPool;

    int m_BackBuffer;
    bool m_Active;
    std::mutex m_Mutex;

private: /* Methods */
    void ResetMedia();

    // Cache
    bool Buffered(v_frame_t frame) const { return m_Buffered.find(frame) != m_Buffered.end(); }
    bool Maxxed() const { return m_Numbers.size() >= m_Capacity; }
    bool Request(v_frame_t frame, bool evict = false);
    void EnsureCached(v_frame_t frame);
    bool Completed() const;
    void EvictFront();
    void EvictBack();
    void Update();
    void CacheAvailable();
    void Cache(v_frame_t frame);
    void Store(v_frame_t frame);
    void CacheNext();
    void CachePrevious();

    inline void AddTask(QRunnable* runnable, int priority = 0) { m_ThreadPool.start(runnable, priority); }

    // Cache frames
    v_frame_t GetNextFrame();
    v_frame_t GetPreviousFrame();
    void CacheNextFrame();
    void CachePreviousFrame();

    // Slots
    void SettingsUpdated();

    friend class CacheNextFrameTask;
    friend class CachePreviousFrameTask;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_VIEWER_BUFFER_H
