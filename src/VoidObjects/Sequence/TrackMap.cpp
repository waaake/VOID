// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm> // for std::lower_bound

/* Internal */
#include "TrackMap.h"

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
    auto it = std::find_if(m_Items.begin(), m_Items.end(), [item](const SharedTrackItem& _i) { return item.get() == _i.get(); });
    return (it == m_Items.end()) ? std::string::npos : static_cast<std::size_t>(it - m_Items.begin());
}

std::size_t TrackMap::ItemIndex(const TrackItem* item) const
{
    auto it = std::find_if(m_Items.begin(), m_Items.end(), [item](const SharedTrackItem& _i) { return item == _i.get(); });
    return (it == m_Items.end()) ? std::string::npos : static_cast<std::size_t>(it - m_Items.begin());
}

SharedTrackItem TrackMap::At(const int frame) const
{
    // Returns the iter to the first item whose timeline in is higher than the requested O(log n)
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
        return nullptr;

    SharedTrackItem item = *(--it);
    return (item->InTimelineRange(frame)) ? item : nullptr;
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

VOID_NAMESPACE_CLOSE
