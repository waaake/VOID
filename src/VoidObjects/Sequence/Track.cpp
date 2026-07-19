// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring> /* For using std::memcpy */
#include <algorithm> /* For std::find and std::lower_bound */
#include <cmath> /* std::abs */

/* Internal */
#include "Track.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* Track Map {[{ */

void TrackMap::Add(const SharedTrackItem& item)
{
    /**
     * The mapping is saved based on the first frame
     * There cannot be a scenario where one track can have multiple track items
     * at the same frame
     */
    m_Items[item->TimelineIn()] = item;

    /* Add the same frame to the vector */
    m_Frames.push_back(item->TimelineIn());
}

bool TrackMap::Add(const SharedTrackItem& item, v_frame_t frame)
{
    if (At(frame))
        return false;

    m_Items[frame] = item;
    m_Frames.push_back(frame);
    return true;
}

void TrackMap::Remove(const SharedTrackItem& item)
{
    /* Remove the item from the underlying structs */
    m_Items.erase(item->TimelineIn());

    /* Remove item from the vector */
    std::vector<int>::iterator it = std::find(m_Frames.begin(), m_Frames.end(), item->TimelineIn());
    if (it != m_Frames.end())
        m_Frames.erase(it);
}

void TrackMap::Remove(v_frame_t frame)
{
    m_Items.erase(frame);
    std::vector<int>::iterator it = std::find(m_Frames.begin(), m_Frames.end(), frame);
    if (it != m_Frames.end())
        m_Frames.erase(it);
}

void TrackMap::Clear()
{
    /* Clears the underlying structures */
    m_Frames.clear();
    m_Items.clear();
}

SharedTrackItem TrackMap::AtIndex(std::size_t index) const
{
    int frame = m_Frames.at(index);
    return m_Items.at(frame);
}

SharedTrackItem TrackMap::At(const int frame) const
{
    /**
     * To get the item at any given frame
     * We first check if we're already in a best case scenario and there is a track item already
     * which starts at this frame
     */
    if (m_Items.find(frame) != m_Items.end())
        return m_Items.at(frame);

    /**
     * If we were not able to find the item directly at the given frame
     * then find the nearest lower frame which is available in the underlying struct
     * This would allow us to get a track item which starts at a frame which is just lower than the current
     * provided frame
     */
    auto it = std::lower_bound(m_Frames.begin(), m_Frames.end(), frame);

    /* if we're at the first index, this means that there is no frame lower than the asked frame in the struct */
    if (it == m_Frames.begin())
        return nullptr;

    /**
     * Now that we have a lower bound frame available, that means we have a track item available
     * But to see if the track item is the correct one, it depends on whether the requested frame is available
     * within the track item's frame bounds
     */
    SharedTrackItem item = m_Items.at(*(--it));

    /* The frame (with the offset applied back to match the media range) is in range of the item's media */
    if (item->InRange(frame))
        return item;

    /* There wasn't any track item at the requested frame */
    return nullptr;
}

bool TrackMap::Move(SharedTrackItem& item, int frame)
{
    // Check that we don't already have an item at the new location for the item
    SharedTrackItem existing = At(frame);
    if (existing && existing.get() != item.get())
        return false;

    // Remove before we add it again to the new location
    Remove(item);
    item->Move(frame);
    Add(item);

    return true;
}

/* }}} */

PlaybackTrack::PlaybackTrack(QObject* parent)
    : VoidObject(parent)
    , m_Recent(nullptr)
    , m_Name("")
    , m_StartFrame(0)
    , m_EndFrame(0)
    , m_Duration(0)
    , m_Visible(true)
    , m_Enabled(true)
    , m_Locked(false)
    // , m_Color(130, 110, 190)    /* Default Purple */
{
    VOID_LOG_INFO("Track Created: {0}", Vuid());
    connect(this, &PlaybackTrack::updated, this, [this]() -> void { m_Recent.reset(); });
}

PlaybackTrack::~PlaybackTrack()
{
}

void PlaybackTrack::SetMedia(const SharedMediaClip& media)
{
    /* Block any signals till the operation has been completed */
    bool b = blockSignals(true);
    Clear();
    blockSignals(b);

    /**
     * When the media gets added, the update and rangeChanged signals are emitted accordingly
     * hence this operation is being performed after the signals have been unblocked and we'll
     * let the other method handle the signals for this operation
     */
    AddMedia(media);
}

void PlaybackTrack::AddMedia(const SharedMediaClip& media)
{
    /* Calculate the offset for reaching the first frame of media in the given timeline */
    int offset = media->FirstFrame() - m_Duration;

    /* Update the duration of the track */
    m_Duration = m_Duration + media->Duration();

    /* Update the last frame of the Track */
    m_EndFrame = m_Duration - 1;

    /* Construct the trackItem with the given media */
    /*
     * The current track is being passed as the parent indicating this track is the parent of the track item
     */
    SharedTrackItem trackItem = std::make_shared<TrackItem>(
                                        media,
                                        media->FirstFrame() - offset,
                                        media->LastFrame() - offset,
                                        offset,
                                        this
                                    );

    // /* Set a Color on the underlying media Clip that it has been associated with this track */
    // trackItem->SetColor(m_Color);

    // /* Connect to Allow frameCache signal be invoked when media in the track item is cached */
    // connect(trackItem.get(), &TrackItem::frameCached, this, [this](int frame) { emit frameCached(frame); });

    /**
     * When the media gets added, it always gets added towards the right side which means the start frame
     * would never change in this case and only would result in a change in the last frame
     * which gets calculated based on previous last frame
     */
    m_Items.Add(trackItem);

    /**
     * Since the media is getting added to the start frame should just remain the same,
     * while the updated end frame along with the original start should get emitted to notify others
     */
    emit rangeChanged(m_StartFrame, m_EndFrame);
}

SharedMediaClip PlaybackTrack::Media(v_frame_t frame)
{
    if (const auto& item = GetTrackItem(frame))
        return item->GetMedia();

    return nullptr;
}

void PlaybackTrack::Clear()
{
    /**
     * Clearing a Track means we are getting rid of all the media that has
     * been added to the track first, once the media is cleared from the store
     * the range of the track needs to be reset to the default range of 0 - 0
     *
     * After both of the operations are performed we emit rangeChanged and updated signal
     * to any listeners
     */
    m_Items.Clear();

    /* This emits the rangeChanged signal */
    SetRange(0, 0, false);

    /* Emit the cleared signal to denote that the track has been cleared of any medias */
    emit cleared();
}

void PlaybackTrack::SetRange(int start, int end, const bool inclusive)
{
    /* Update the internal frames */
    m_StartFrame = start;
    m_EndFrame = end;

    /**
     * If inclusive is true we include the last frame in duration calculation
     *
     * so if my start frame is 1001
     * and my end frame is 1010
     * if we're not inclusive my duration is
     * 1010 - 1001 = 9 frames
     * but if we're inclusive of the last frame then the duration becomes
     * (1010 - 1001) + 1 = 10 frames
     */
    m_Duration = (end - start) + static_cast<int>(inclusive); // inclusive is bool so will be casted 0 or 1

    /* Emit the signal the the range has been modified */
    emit rangeChanged(m_StartFrame, m_EndFrame);
}

void PlaybackTrack::Cache(v_frame_t frame)
{
    if (auto item = GetTrackItem(frame))
        item->GetMedia()->Cache(frame);
}

void PlaybackTrack::Image(v_frame_t frame, FloatImage& image)
{
    if (auto item = GetTrackItem(frame))
        item->Image(frame, image);
}

const FloatImage PlaybackTrack::Image(v_frame_t frame)
{
    if (auto item = GetTrackItem(frame))
        return item->Image(frame);

    return nullptr;
}

void PlaybackTrack::ClearCache()
{
    for (SharedTrackItem& item: m_Items)
        item->GetMedia()->ClearCache();
}

void PlaybackTrack::ClearCache(v_frame_t frame)
{
    if (auto item = GetTrackItem(frame))
        item->ClearCache(frame);
}

v_frame_t PlaybackTrack::GetSnapFrame(v_frame_t frame, const SharedTrackItem& trackitem, int threshold) const
{
    for (int i = 0; i < static_cast<int>(m_Items.Size()); ++i)
    {
        const SharedTrackItem& item = m_Items.AtIndex(i);

        if (item.get() == trackitem.get())
            continue;

        // No valid frame to snap to
        if (item->TimelineIn() > frame)
            return -1;

        if (std::abs(frame - item->TimelineOut()) < threshold)
            return item->TimelineOut() + 1;
    }

    return -1;
}

SharedTrackItem PlaybackTrack::GetTrackItem(v_frame_t frame)
{
    if (m_Recent && m_Recent->InRange(frame))
        return m_Recent;

    m_Recent = m_Items.At(frame);
    return m_Recent;
}

bool PlaybackTrack::MoveItem(SharedTrackItem& item, v_frame_t frame)
{
    return m_Items.Move(item, frame);
}

bool PlaybackTrack::AddItem(SharedTrackItem& item, v_frame_t frame)
{
    if (m_Items.Add(item, frame))
    {
        item->setParent(this);
        // Set the timeline range based on the provided frame
        item->Move(frame);
        emit updated();

        return true;
    }
    return false;
}

void PlaybackTrack::RemoveItem(v_frame_t frame)
{
    m_Items.Remove(frame);
    emit mediaRemoved();
    emit updated();
}

void PlaybackTrack::RemoveItem(SharedTrackItem& item)
{
    m_Items.Remove(item);
    emit mediaRemoved();
    emit updated();
}

VOID_NAMESPACE_CLOSE
