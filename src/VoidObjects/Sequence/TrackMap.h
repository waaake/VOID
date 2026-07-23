// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TRACK_MAP_H
#define _TRACK_MAP_H

/* STD */
#include <memory>
#include <map>
#include <vector>

/* Internal */
#include "Definition.h"
#include "TrackItem.h"

VOID_NAMESPACE_OPEN

class PlaybackTrack;

/*
 * Let the Tracks be auto managed by reference count whether they survive
 * in the world or die or even get killed
 */
typedef std::shared_ptr<PlaybackTrack> SharedPlaybackTrack;

/**
 * @brief Stores items in a Track.
 * The items need to be sorted all the time based on the timeline in.
 * Hence the complexities are,
 * 
 * Insertion at the end O(1)
 * Insertion in between O(log n)
 * Accessing an item is O(log n)
 * 
 */
class VOID_API TrackMap
{
public:
    /**
     * Adds track item to the mapping.
     */
    void Add(const SharedTrackItem& item);
    bool Add(const SharedTrackItem& item, v_frame_t frame);

    /**
     * Removes the track item from the mapping.
     */
    void Remove(const SharedTrackItem& item);
    void Remove(v_frame_t frame);
    void Clear() { m_Items.clear(); }

    std::size_t Size() const { return m_Items.size(); }
    SharedTrackItem AtIndex(std::size_t index) const { return m_Items.at(index); }

    /**
     * Returns a Track Item present at a given frame, if it exists
     * else a null pointer is returned.
     */
    SharedTrackItem At(const int frame) const;
    inline bool Empty() const { return m_Items.empty(); }

    bool Move(SharedTrackItem& item, int frame);
    const std::vector<SharedTrackItem>& Items() const { return m_Items; }

    std::vector<SharedTrackItem>::iterator begin() noexcept { return m_Items.begin(); }
    std::vector<SharedTrackItem>::iterator end() noexcept { return m_Items.end(); }
    std::vector<SharedTrackItem>::const_iterator cbegin() const noexcept { return m_Items.cbegin(); }
    std::vector<SharedTrackItem>::const_iterator cend() const noexcept { return m_Items.cend(); }

private:
    std::vector<SharedTrackItem> m_Items;
};

VOID_NAMESPACE_CLOSE

#endif // _TRACK_MAP_H
