// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_IMAGE_BUFFER_H
#define _VOID_IMAGE_BUFFER_H

/* STD */
#include <deque>
#include <vector>
#include <algorithm>
#include <unordered_map>

/* Qt */
#include <QThreadPool>
#include <QRunnable>

/* Internal */
#include "Definition.h"
#include "VoidCore/Media.h"

VOID_NAMESPACE_OPEN

/* Forward Decl */
class Player;

/**
 * Class holding Media frame caches and tracking the cache against the maximum
 * allowed memory usage for caching the image/movie frames.
 */
class FrameBuffer : public QThreadPool
{
    Q_OBJECT

public:
    FrameBuffer(Player* player, QObject* parent = nullptr);
    ~FrameBuffer();

    inline void SetMaxMemory(unsigned int size_in_gigs) { m_MaxMemory = size_in_gigs * 1024 * 1024 * 1024; }

    void SetMedia(Media* media);

    /**
     * A Cache process which caches all frames till the memory size allows
     */
    void CacheAvailableFrames();

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
    bool Request(bool evict = false);

    /**
     * Cache the provided frame for the media, if the frame is already cached nothing happens in terms
     * of memory usage
     */
    void Cache(v_frame_t frame);

    /**
     * Based on the direction this returns the minimum frame (or maximum) frame which is the candidate for
     * eviction in case that is necessary
     */
    inline v_frame_t MinFrame() const 
    { 
        if (m_Framenumbers.empty())
            return StartFrame();

        return m_Framenumbers.front(); 
    }

    inline v_frame_t StartFrame() const { return m_Media->FirstFrame(); }
    inline v_frame_t EndFrame() const { return m_Media->LastFrame(); }

    v_frame_t CurrentFrame() const;
    void EnsureCached(v_frame_t frame);

    void Clear();

private: /* Members */
    /**
     * This is a non-owning pointer to the Media coming in from either the viewer buffer or some other
     * component that might need the media to be cache for some reason
     * this class is never to be held responsible for managing the Media in terms of memory, managing media
     * is solely the responsiblity of the component providing the media pointer here
     */
    Media* m_Media;

    /**
     * Bytes representation of the amount of maximum available memory for caching media
     * if the memory is full then the caching is stopped unless a force request (evict = true) is made
     * this makes the last first or frame based on direction be evicted (cleared of any cached data) from memory
     */
    size_t m_MaxMemory;
    size_t m_UsedMemory;

    size_t m_FrameSize;

    std::deque<v_frame_t> m_Framenumbers;
    std::unordered_map<v_frame_t, Frame*> m_Frames;

    Player* m_Player;

private: /* Methods */
    void Evict(v_frame_t frame);
};

class CacheAvailableFramesTask : public QRunnable
{
public:
    CacheAvailableFramesTask(FrameBuffer* framebuffer);
    void run() override;

private:
    FrameBuffer* m_FrameBuffer;
};

class CacheAvailableMovieFramesTask : public QRunnable
{
public:
    CacheAvailableMovieFramesTask(FrameBuffer* framebuffer);
    void run() override;

private:
    FrameBuffer* m_FrameBuffer;
};

class CacheFramesTask : public QRunnable
{
public:
    CacheFramesTask(FrameBuffer* framebuffer, v_frame_t frame);
    void run() override;

private:
    FrameBuffer* m_FrameBuffer;
    v_frame_t m_Frame;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_BUFFER_H
