// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm> // for std::lower_bound

/* Internal */
#include "TrackMap.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

void TrackMap::Add(const SharedTrackItem& item)
{
    const auto it = std::lower_bound(
        m_Items.begin(), 
        m_Items.end(),
        item->TimelineIn(),
        [](const SharedTrackItem& _i, v_frame_t _f)
        {
            return _i->TimelineIn() <= _f;
        }
    );

    m_Items.insert(it, item);
}

bool TrackMap::Add(const SharedTrackItem& item, v_frame_t frame)
{
    auto it = std::lower_bound(
        m_Items.begin(),
        m_Items.end(),
        frame,
        [](const SharedTrackItem& _i, v_frame_t _f)
        {
            return _i->TimelineIn() <= _f;
        }
    );

    if (it == m_Items.begin())
    {
        m_Items.insert(it, item);
        return true;
    }

    SharedTrackItem existing = *(it - 1);
    if (existing->InTimelineRange(frame))
        return false;

    m_Items.insert(it, item);
    return true;
}

void TrackMap::Remove(const SharedTrackItem& item)
{
    m_Items.erase(
        std::remove_if(
            m_Items.begin(),
            m_Items.end(),
            [item](const SharedTrackItem& _i)
            {
                return _i.get() == item.get();
            }
        ),
        m_Items.end()
    );
}

void TrackMap::Remove(v_frame_t frame)
{
    m_Items.erase(
        std::remove_if(
            m_Items.begin(),
            m_Items.end(),
            [frame](const SharedTrackItem& _i)
            {
                return _i->TimelineIn() == frame;
            }
        ),
        m_Items.end()
    );
}

std::size_t TrackMap::ItemIndex(const SharedTrackItem& item) const
{
    /// The internal items are all arranged in ascending order of the timeline in, so we can run binary search as opposed to linear
    v_frame_t start = item->TimelineIn();

    int low = 0;
    int high = static_cast<int>(m_Items.size()) - 1;

    while (low <= high)
    {
        const int mid = low + (high - low) / 2;
        if (m_Items[mid]->TimelineIn() < start)
            low = mid + 1;
        else if (m_Items[mid]->TimelineIn() > start)
            high = mid - 1;
        else
            return mid;
    }

    return std::string::npos;
}

std::size_t TrackMap::ItemIndex(const TrackItem* item) const
{
    /// The internal items are all arranged in ascending order of the timeline in, so we can run binary search as opposed to linear
    v_frame_t start = item->TimelineIn();

    int low = 0;
    int high = static_cast<int>(m_Items.size()) - 1;

    while (low <= high)
    {
        const int mid = low + (high - low) / 2;
        if (m_Items[mid]->TimelineIn() < start)
            low = mid + 1;
        else if (m_Items[mid]->TimelineIn() > start)
            high = mid - 1;
        else
            return mid;
    }

    return std::string::npos;
}

SharedTrackItem TrackMap::At(const int frame) const
{
    int low = 0;
    int high = static_cast<int>(m_Items.size()) - 1;

    while (low <= high)
    {
        const int mid = low + (high - low) / 2;
        const SharedTrackItem& item = m_Items[mid];
        const MFrameRange range = item->TimelineRange();
        if (range.Contains(frame))
            return item;
        else if (range.endframe < frame)
            low = mid + 1;
        else if (range.startframe > frame)
            high = mid - 1;
    }

    return nullptr;
}

SharedTrackItem TrackMap::InRange(v_frame_t start, v_frame_t end) const
{
    int low = 0;
    int high = static_cast<int>(m_Items.size()) - 1;

    const MFrameRange source(start, end);
    while (low <= high)
    {
        const int mid = low + (high - low) / 2;
        const SharedTrackItem& item = m_Items[mid];
        const MFrameRange range = item->TimelineRange();

        if (source.Covers(range) || source.Overlaps(range) || range.Covers(source))
            return item;
        else if (range.endframe < start)
            low = mid + 1;
        else if (range.startframe > end)
            high = mid - 1;
    }

    return nullptr;
}

bool TrackMap::Move(const SharedTrackItem& item, int frame)
{
    auto it = std::lower_bound(
        m_Items.begin(),
        m_Items.end(),
        frame,
        [](const SharedTrackItem& _i, v_frame_t _f)
        {
            return _i->TimelineIn() <= _f;
        }
    );

    SharedTrackItem existing = (it == m_Items.begin()) ? *(it) : *(--it);
    if (existing->InTimelineRange(frame))
    {
        // This is some other item, we're dealing with
        if (existing.get() != item.get())
            return false;
    }

    item->Move(frame);

    /// This is slightly more expensive than initially thought
    /// Every move should also update the vector such that the moved track item now is at
    /// an index in which it's range sits correctly as the rest

    // To make things a bit better, we try to sort only the range from current item's placement
    // to the point where we have a track item just higher than it
    // We already know the item that's just lesser than the current item's 

    /// Should not be a case where we can't find the item as we just sorted that :D
    auto cit = std::find(m_Items.begin(), m_Items.end(), item);
    Sort(static_cast<int>(cit - m_Items.begin()), static_cast<int>(it - m_Items.begin()));
    return true;
}

bool TrackMap::Offset(const SharedTrackItem& item, int offset)
{
    auto it = std::lower_bound(
        m_Items.begin(),
        m_Items.end(),
        item->TimelineIn() + offset,
        [](const SharedTrackItem& _i, v_frame_t _f)
        {
            return _i->TimelineIn() <= _f;
        }
    );

    SharedTrackItem existing = (it == m_Items.begin()) ? *(it) : *(--it);
    if (existing->InTimelineRange(item->TimelineIn() + offset))
    {
        // Some other Item exists
        if (existing.get() != item.get())
            return false;
    }

    item->Offset(offset);
    return true;
}

void TrackMap::Sort()
{
    std::sort(m_Items.begin(), m_Items.end(), [](const SharedTrackItem& _a, const SharedTrackItem& _b) -> bool
    {
        return _a->TimelineIn() < _b->TimelineIn();
    });
}

void TrackMap::Sort(int start, int end)
{
    /// +1 incremented on the end to include the last element
    if (end < start)
    {
        std::sort(m_Items.begin() + end, m_Items.begin() + start + 1, [](const SharedTrackItem& _a, const SharedTrackItem& _b) -> bool
        {
            return _a->TimelineIn() < _b->TimelineIn();
        });
        return;
    }

    std::sort(m_Items.begin() + start, m_Items.begin() + end + 1, [](const SharedTrackItem& _a, const SharedTrackItem& _b) -> bool
    {
        return _a->TimelineIn() < _b->TimelineIn();
    });
}

VOID_NAMESPACE_CLOSE
