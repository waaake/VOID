// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_VIEWER_BUFFER_H
#define _VOID_VIEWER_BUFFER_H

/* STD */
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
    SharedPixels image = nullptr;
    Renderer::SharedAnnotation annotation = nullptr;

    bool Valid() const { return (bool)image; }
    explicit operator bool() const { return (bool)image; }
};

class ViewerBuffer : public QObject
{
    Q_OBJECT

    class CacheNextFrameTask : public QRunnable
    {
    public:
        explicit CacheNextFrameTask(ViewerBuffer* parent) : m_Parent(parent) {}
        inline void run() override { m_Parent->CacheNextFrame(); }

    private:
        ViewerBuffer* m_Parent;
    };

    class CachePreviousFrameTask : public QRunnable
    {
    public:
        explicit CachePreviousFrameTask(ViewerBuffer* parent) : m_Parent(parent) {}
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
        Playlist
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

    SharedPlaybackTrack ActiveTrack() const;

    /**
     * Range for the buffer
     */
    inline v_frame_t StartFrame() const { return m_Startframe; }
    inline v_frame_t EndFrame() const { return m_Endframe; }

    inline void SetActivePlayer(Player* player) { m_Player = player; }
    inline void SetMaxMemory(unsigned long long gigs) { m_MaxMemory = gigs * 1024 * 1024 * 1024; }
    inline void SetMaxThreads(unsigned int count) { m_ThreadPool.setMaxThreadCount(count); }

    void StartPlaybackCache(const PlayState& state = PlayState::Forwards);
    inline void RestartPlaybackCache() { StartPlaybackCache(m_State); }
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

    /**
     * Returns the current Component Type which is playing in the buffer
     */
    inline PlayableComponent PlayingComponent() const { return m_PlayingComponent; }

    /**
     * Returns a track item at a given frame
     */
    SharedTrackItem TrackItem(const v_frame_t frame);

    /**
     * Returns the Image Pixels from the active item in the buffer
     */
    SharedPixels Image(const v_frame_t frame);

    /**
     * @brief Returns the active media's buffer data which includes the image data
     * and the annotations if available on the media
     *
     * @param frame Frame number.
     * @param nearest If the frame does not exist, whether a nearest available frame's data is required
     * @return BufferData Media data from the active media.
     */
    BufferData MData(const v_frame_t frame, bool nearest = false);

    /**
     * @brief Returns the Media from the active component.
     *
     * @param frame Frame number.
     * @return SharedMediaClip Media clip from the active component at the given frame.
     */
    SharedMediaClip Media(const v_frame_t frame);

    /**
     * Returns the shared media clip instance
     */
    inline SharedMediaClip GetMediaClip() const { return m_Clip; }

    /**
     * Returns the shared media track instance
     */
    inline SharedPlaybackTrack GetTrack() const { return m_Track; }

    /**
     * Returns the shared sequence instance
     */
    inline SharedPlaybackSequence GetSequence() const { return m_Sequence; }

    /**
     * Returns whether the provided media clip is currently being played
     */
    bool Playing(const SharedMediaClip& media) const;

    /**
     * Active state of the Viewer Buffer
     */
    [[nodiscard]] inline bool Active() const { return m_Active; }
    inline void SetActive(const bool active) { m_Active = active; emit updated(); }

    /**
     * Name of the viewer buffer
     */
    inline std::string Name() const { return m_Name; }
    inline void SetName(const std::string& name) { m_Name = name; }

    /**
     * Color Associtated with the viewer buffer to indicate its reference across the UI
     */
    inline QColor Color() const { return m_Color; }
    void SetColor(const QColor& color);

    /**
     * Refreshes any underlying caches and updates internals
     */
    void Refresh();

    /**
     * Clears any playable medias from the buffer
     * this might be needed when a media which is currently playing was removed
     */
    void Clear();

    /**
     * Set a playable component on the Buffer
     */
    void Set(const SharedMediaClip& media);
    void Set(const SharedPlaybackTrack& track);
    void Set(const SharedPlaybackSequence& sequence);
    void Set(const std::vector<SharedMediaClip>& media);

    void SetPlaylist(Playlist* playlist);
    [[nodiscard]] bool NextMedia();
    [[nodiscard]] bool PreviousMedia();

    /**
     * Set Annotation on the Active Media Item
     */
    void SetAnnotation(const v_frame_t, const Renderer::SharedAnnotation& annotation);
    /**
     * Removes Annotation from the Active Media item
     */
    void RemoveAnnotation(const v_frame_t);

signals:
    void updated();

private: /* Members */
    /**
     * Playable Entities
     * Obviously one will get played at a time but can be governed what gets played
     */
    SharedMediaClip m_Clip;
    SharedPlaybackTrack m_Track;
    SharedPlaybackSequence m_Sequence;
    Playlist* m_Playlist;

    /**
     * At any point this buffer maintains a cached track item which makes the query to get the track item
     * at a given frame much less complex in terms of time
     * The reasoning is, when a standard play operation happens, it happnes sequentially
     * and a track item at any given frame, if queried can last upto some frames ahead of it (till it's range supports)
     * so the next query to the underlying struct only happens when this cached track item is out of frames based on the
     * requested frame and then returned item is then cached till a frame is requested outside and so on...
     */
    SharedTrackItem m_CachedTrackItem;

    PlayableComponent m_PlayingComponent;
    PlayState m_State;

    std::string m_Name;
    QColor m_Color;

    Player* m_Player;
    QThreadPool m_ThreadPool;

    /**
     * Bytes representation of the amount of maximum available memory for caching media
     * if the memory is full then the caching is stopped unless a force request (evict = true) is made
     * this makes the last first or frame based on direction be evicted (cleared of any cached data) from memory
     */
    std::size_t m_MaxMemory;
    std::size_t m_UsedMemory;
    std::size_t m_FrameSize;

    v_frame_t m_Startframe, m_Endframe;
    v_frame_t m_LastCached;

    int m_BackBuffer;
    bool m_Active;

    QTimer m_CacheTimer;
    std::mutex m_Mutex;

    std::deque<v_frame_t> m_Framenumbers;
    std::unordered_set<v_frame_t> m_Buffered;

private: /* Methods */
    /**
     * Returns a track item from the track or sequence at a given frame
     * The item returned from the entity is then cached locally on the class to reduce the operations to try and find
     * a track item for the next frame (unless the frame is out of items' bounds)
     */
    SharedTrackItem ItemFromTrack(const v_frame_t frame);
    SharedTrackItem ItemFromSequence(const v_frame_t frame);

    BufferData ClipData(const v_frame_t frame, bool nearest = false);
    BufferData TrackData(const v_frame_t frame, bool nearest = false);
    BufferData SequenceData(const v_frame_t frame, bool nearest = false);

    v_frame_t InternalStartframe() const;
    v_frame_t InternalEndframe() const;
    int InternalDuration() const;

    bool Completed() const;

    inline std::size_t AvailableMemory() const { return m_MaxMemory - m_UsedMemory; }

    /**
     * Frame Eviction from the cached array
     */
    void EvictFront();
    void EvictBack();

    /**
     * Update to refresh the cache to available frames after removing the frames
     * that may no longer be required
     */
    void Update();

    void UpdateRange(v_frame_t start, v_frame_t end);
    inline void AddTask(QRunnable* runnable, int priority = 0) { m_ThreadPool.start(runnable, priority); }
    inline bool Cached(v_frame_t frame) const { return m_Buffered.find(frame) != m_Buffered.end(); }

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
    void Store(v_frame_t frame);

    v_frame_t GetNextFrame();
    v_frame_t GetPreviousFrame();
    void CacheNextFrame();
    void CachePreviousFrame();
    void CacheNext();
    void CachePrevious();

    void SettingsUpdated();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_VIEWER_BUFFER_H
