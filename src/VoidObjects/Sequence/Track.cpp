// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring> /* For using std::memcpy */
#include <algorithm> /* For std::find and std::lower_bound */

/* Internal */
#include "Track.h"

VOID_NAMESPACE_OPEN

/* Track Map {[{ */

void TrackMap::Add(const SharedTrackItem& item)
{
    /**
     * The mapping is saved based on the first frame
     * There cannot be a scenario where one track can have multiple track items
     * at the same frame
     */
    m_Items[item->StartFrame()] = item;

    /* Add the same frame to the vector */
    m_Frames.push_back(item->StartFrame());
}

void TrackMap::Remove(const SharedTrackItem& item)
{
    /* Remove the item from the underlying structs */
    m_Items.erase(item->StartFrame());

    /* Remove item from the vector */
    std::vector<int>::iterator it = std::find(m_Frames.begin(), m_Frames.end(), item->StartFrame());

    /* If the frame exists -> remove it */
    if (it != m_Frames.end())
        m_Frames.erase(it);
}

void TrackMap::Clear()
{
    /* Clears the underlying structures */
    m_Frames.clear();
    m_Items.clear();
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
    if (item->HasFrame(frame))
        return item;

    /* There wasn't any track item at the requested frame */
    return nullptr;
}

/* }}} */

PlaybackTrack::PlaybackTrack(QObject* parent)
    : VoidObject(parent)
    , m_StartFrame(0)
    , m_EndFrame(0)
    , m_Duration(0)
    , m_Visible(true)
    , m_Enabled(true)
    , m_Color(130, 110, 190)    /* Default Purple */
{
    VOID_LOG_INFO("Track Created: {0}", Vuid());
}

PlaybackTrack::~PlaybackTrack()
{
}

void PlaybackTrack::SetMedia(const SharedMediaClip& media)
{
    /* Block any signals till the operation has been completed */
    bool b = blockSignals(true);

    /* Clear any Media present */
    Clear();

    /* Unblock signals */
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

    /* Set a Color on the underlying media Clip that it has been associated with this track */
    trackItem->SetColor(m_Color);
    
    /* Connect to Allow frameCache signal be invoked when media in the track item is cached */
    connect(trackItem.get(), &TrackItem::frameCached, this, [this](int frame) { emit frameCached(frame); });

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

void PlaybackTrack::Cache()
{
    /* For each of the track item in the underlying array -> Cache the item's media */
    for (SharedTrackItem& item: m_Items)
    {
        /* This emits the frameCached signal for each frame that has been cached */
        item->Cache();
    }
}

void PlaybackTrack::ClearCache()
{
    /* Clear cache for each items' media */
    for (SharedTrackItem& item: m_Items)
    {
        /* Clear Cache */
        item->GetMedia()->ClearCache();
    }

    /* Once all media has been rid of cache -> emit the cacheCleared signal */
    emit cacheCleared();
}

VOID_NAMESPACE_CLOSE
