// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_VIEWER_BUFFER_H
#define _VOID_VIEWER_BUFFER_H

/* STD */
#include <atomic>
#include <deque>
#include <memory>
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
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Playlist/Playlist.h"

VOID_NAMESPACE_OPEN

/* Forward decl for Player as this needs to know the current playback type and frame */
class Player;

/**
 * Enum decribing which viewer buffer is currently active and can be used to set an
 * active viewer buffer for the
 */
enum class VOID_API PlayerViewBuffer
{
    /* Relates to ViewerBuffer A */
    A,
    /* Relates to ViewerBuffer B */
    B

    /* In Future the Buffer type may also include Compare buffers like switch/swipe and others. */
};

struct BufferData
{
    FloatImage image = nullptr;
    Renderer::SharedAnnotation annotation = nullptr;

    bool Valid() const { return (bool)image; }
    explicit operator bool() const { return (bool)image; }
};

class VOID_API ViewerBuffer : public QObject
{
    Q_OBJECT

    class CacheNextFrameTask : public QRunnable
    {
    public:
        CacheNextFrameTask(ViewerBuffer* parent) : m_Parent(parent) {}
        inline void run() override { m_Parent->CacheNextFrame(); }

    private:
        ViewerBuffer* m_Parent;
    };

    class CachePreviousFrameTask : public QRunnable
    {
    public:
        CachePreviousFrameTask(ViewerBuffer* parent) : m_Parent(parent) {}
        inline void run() override { m_Parent->CachePreviousFrame(); }

    private:
        ViewerBuffer* m_Parent;
    };

public: /* Enums */
    /**
     * Describes the playing components on the Buffer
     * The components are Media Clip
     * Track from a Sequence
     * The entire sequence which means all tracks are included and their priority
     * are controlled by the sequence itself
     */
    enum class PlayableComponent
    {
        Sequence,
        Track,
        Clip,
        Playlist,
        Grid
    };

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

    // Attribs
    inline v_frame_t StartFrame() const { return m_Startframe; }
    inline v_frame_t EndFrame() const { return m_Endframe; }
    inline PlayableComponent PlayingComponent() const { return m_PlayingComponent; }
    [[nodiscard]] inline bool Active() const { return m_Active; }
    inline std::string Name() const { return m_Name; }
    inline QColor Color() const { return m_Color; }

    // Entities
    SharedPlaybackTrack ActiveTrack() const;
    SharedTrackItem TrackItem(const v_frame_t frame);
    SharedMediaClip Media(const v_frame_t frame);
    inline SharedMediaClip GetMediaClip() const { return m_Clip; }
    inline SharedPlaybackTrack GetTrack() const { return m_Track; }
    inline SharedPlaybackSequence GetSequence() const { return m_Sequence; }

    // Set Attribs
    inline void SetMaxThreads(unsigned int count) { m_ThreadPool.setMaxThreadCount(count); }
    void SetFramesize(std::size_t size);
    void SetMaxMemory(std::size_t gigabytes);

    inline void SetActive(const bool active) { m_Active = active; emit updated(); }
    inline void SetName(const std::string& name) { m_Name = name; }
    void SetColor(const QColor& color);

    // Set Components
    inline void SetActivePlayer(Player* player) { m_Player = player; }

    // Cache/Buffer
    bool Buffered(v_frame_t frame) const { return m_Buffered.find(frame) != m_Buffered.end(); }
    bool Maxxed() const { return m_Numbers.size() >= m_Capacity; }
    bool Request(v_frame_t frame, bool evict = false);
    void StartPlaybackCache(const PlayState& state = PlayState::Forwards);
    inline void RestartPlaybackCache() { StartPlaybackCache(m_State); }
    void StopPlaybackCache();
    void PauseCaching();
    void DisableCaching();
    void StopCaching();
    void ResumeCaching();
    void Recache();
    void ClearCache();
    void EnsureCached(v_frame_t frame);

    void Refresh();
    void Clear();

    // ImageData
    const FloatImage Image(const v_frame_t frame);
    BufferData MData(const v_frame_t frame, bool nearest = false);
    std::vector<FloatImage> GridFrame(const v_frame_t frame);

    bool Playing(const SharedMediaClip& media) const;

    // Media
    [[nodiscard]] bool NextMedia();
    [[nodiscard]] bool PreviousMedia();
    [[nodiscard]] bool ResetPlaylistMedia();

    // Set Media
    void Set(const SharedMediaClip& media);
    void Set(const SharedPlaybackTrack& track);
    void Set(const SharedPlaybackSequence& sequence);
    void Set(const std::vector<SharedMediaClip>& media);
    void SetGrid(Playlist* playlist);
    void SetPlaylist(Playlist* playlist);

    // Media Components
    void SetAnnotation(const v_frame_t, const Renderer::SharedAnnotation& annotation);
    void RemoveAnnotation(const v_frame_t);
    const std::unordered_map<v_frame_t, Renderer::SharedAnnotation>& Annotations() const { return m_Clip->Annotations(); }

signals:
    void updated();
    void playlistUpdated(Playlist*);

private: /* Members */
    std::deque<v_frame_t> m_Numbers;
    std::unordered_set<v_frame_t> m_Buffered;

    // Attribs
    std::string m_Name;
    QTimer m_CacheTimer;
    QColor m_Color;
    Player* m_Player;

    // Media
    SharedMediaClip m_Clip;
    SharedPlaybackTrack m_Track;
    SharedPlaybackSequence m_Sequence;
    Playlist* m_Playlist;

    std::size_t m_MaxMemory;
    std::atomic<std::size_t> m_Framesize;
    std::atomic<unsigned int> m_Capacity;
    v_frame_t m_Startframe, m_Endframe;
    v_frame_t m_LastCached;

    // Enums
    PlayableComponent m_PlayingComponent;
    PlayState m_State;

    QThreadPool m_ThreadPool;

    int m_BackBuffer;
    bool m_Active;
    std::mutex m_Mutex;

private: /* Methods */

    // Attrib
    v_frame_t InternalStartframe() const;
    v_frame_t InternalEndframe() const;
    int InternalDuration() const;
    void UpdateRange(v_frame_t start, v_frame_t end);

    // Cache
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
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_VIEWER_BUFFER_H
