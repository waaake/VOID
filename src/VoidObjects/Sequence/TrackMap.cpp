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
    if (existing->InRange(frame))
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
    return (item->InRange(frame)) ? item : nullptr;
}

bool TrackMap::Move(SharedTrackItem& item, int frame)
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
    if (existing->InRange(frame))
    {
        // This is some other item, we're dealing with
        if (existing.get() != item.get())
            return false;

        m_Items.erase(it);
    }

    item->Move(frame);
    Add(item);
    return true;
}

VOID_NAMESPACE_CLOSE
