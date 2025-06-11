/* STD */
#include <cstring> /* For using std::memcpy */

/* Internal */
#include "Track.h"

VOID_NAMESPACE_OPEN

PlaybackTrack::PlaybackTrack(QObject* parent)
    : QObject(parent)
    , m_StartFrame(0)
    , m_EndFrame(0)
    , m_Visible(true)
    , m_Enabled(true)
    , m_Color(130, 110, 190)    /* Default Purple */
{
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
    int offset = media->FirstFrame() - (m_StartFrame + m_EndFrame);

    /* Update the last frame of the Track */
    m_EndFrame = (m_EndFrame + media->Duration()) - 1;

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
    m_TrackItems.push_back(trackItem);

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
    m_TrackItems.clear();

    /* This emits the rangeChanged signal */
    SetRange(0, 0);

    /* Emit the cleared signal to denote that the track has been cleared of any medias */
    emit cleared();
}

void PlaybackTrack::SetRange(int start, int end)
{
    /* Update the internal frames */
    m_StartFrame = start;
    m_EndFrame = end;

    /* Emit the signal the the range has been modified */
    emit rangeChanged(m_StartFrame, m_EndFrame);
}

bool PlaybackTrack::GetImage(const int frame, VoidImageData* image)
{
    for (SharedTrackItem item: m_TrackItems)
    {
        /**
         * Check whether the current frame + the offset on the trackItem
         * exists in the range of the Media, if so update the pointer to the data with the
         * data from the Media and we can return true indicating a frame is successfully found
         */
        int f = frame + item->GetOffset();
        if (item->GetMedia()->InRange(f))
        {
            /**
             * Check for a case where the frame lies in the range of media but is still not available to read
             * Reason could be that it is missing
             */
            if (!item->GetMedia()->Contains(f))
                return false;

            /**
             * Copy the data of the VoidImageData* which we'll update for anyone to access
             */
            std::memcpy(image, item->GetMedia()->Image(f), sizeof(VoidImageData));

            /**
             * Once the Pointer data is copied ->
             * emit the frameCached signal to indicate that this frame data is now available on the Media Class
             */
            emit frameCached(frame);

            /* The memory address of the provided pointer has been updated with the data from the media */
            return true;
        }
    }

    /* The provided frame from the timeline does not have any trackitem/media on it */
    return false;
}

SharedTrackItem PlaybackTrack::GetTrackItem(const int frame) const
{
    for (SharedTrackItem item: m_TrackItems)
    {
        /**
         * Check whether the current frame + the offset on the trackItem
         * exists in the range of the Media, if so update the pointer to the data with the
         * data from the Media and we can return true indicating a frame is successfully found
         */
        int f = frame + item->GetOffset();
        if (item->GetMedia()->InRange(f))
        {
            return item;
        }
    }

    /* The provided frame from the timeline does not have any trackitem/media on it */
    return nullptr;
}

void PlaybackTrack::Cache()
{
    /* For each of the track item in the underlying array -> Cache the item's media */
    for (SharedTrackItem item: m_TrackItems)
    {
        /* This emits the frameCached signal for each frame that has been cached */
        item->Cache();
    }
}

void PlaybackTrack::ClearCache()
{
    /* Clear cache for each items' media */
    for (SharedTrackItem item: m_TrackItems)
    {
        /* Clear Cache */
        item->GetMedia()->ClearCache();
    }

    /* Once all media has been rid of cache -> emit the cacheCleared signal */
    emit cacheCleared();
}

VOID_NAMESPACE_CLOSE
