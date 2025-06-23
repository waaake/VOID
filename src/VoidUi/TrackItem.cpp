/* Internal */
#include "TrackItem.h"

VOID_NAMESPACE_OPEN

TrackItem::TrackItem(QObject* parent)
    : VoidObject(parent)
    , m_Offset(0)
    , m_StartFrame(0)
    , m_EndFrame(0)
{
    VOID_LOG_INFO("TrackItem Created: {0}", Vuid());
}

TrackItem::TrackItem(const SharedMediaClip& media, int start, int end, int offset, QObject* parent)
    : VoidObject(parent)
    , m_Media(media)
    , m_Offset(offset)
    , m_StartFrame(start)
    , m_EndFrame(end)
{
    VOID_LOG_INFO("TrackItem Created: {0}", Vuid());
}

TrackItem::~TrackItem()
{
}

void TrackItem::SetMedia(const SharedMediaClip& media, int offset)
{
    /* Update the underlying media and relevant offset */
    m_Media = media;
    m_Offset = offset;

    /* Once the Media has been updated -> emit mediaChanged signal */
    emit mediaChanged();
}

void TrackItem::SetRange(int start, int end)
{
    /* 
     * Update the range of the track item
     * This points to where the track item starts and end in a given Track
     */
    m_StartFrame = start;
    m_EndFrame = end;
}

void TrackItem::Cache()
{
    /* For each frame in Media -> Cache the frame and emit the signal that a frame has been cached */
    for (std::pair<const int, ImageFrame>& it: m_Media->GetMedia())
    {
        /* Cache the data for the frame */
        it.second.Cache();

        /* Emit the frame - the offset for the track item */
        emit frameCached(it.first - m_Offset);
    }
}

SharedPixBlock TrackItem::GetImage(const int frame)
{
    /* Update the frame value with the offset so that we match the original media range */
    int f = frame + m_Offset;

    if (m_Media->Contains(f))
    {
        /* Emit that the frame from the timeline was cached -- in case it was not before */
        emit frameCached(frame);

        return m_Media->Image(f);
    }

    /* The provided frame is not present in the Media */
    return nullptr;
}

VOID_NAMESPACE_CLOSE
