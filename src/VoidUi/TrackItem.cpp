/* Internal */
#include "TrackItem.h"

VOID_NAMESPACE_OPEN

TrackItem::TrackItem(QObject* parent)
    : QObject(parent)
    , m_Offset(0)
    , m_StartFrame(0)
    , m_EndFrame(0)
{
}

TrackItem::TrackItem(const Media& media, int start, int end, int offset, QObject* parent)
    : QObject(parent)
    , m_Media(media)
    , m_Offset(offset)
    , m_StartFrame(start)
    , m_EndFrame(end)
{
}

TrackItem::~TrackItem()
{
}

void TrackItem::SetMedia(const Media& media, int offset)
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
    for (std::pair<const int, Frame>& it: m_Media)
    {
        /* Cache the data for the frame */
        it.second.Cache();

        /* Emit the frame - the offset for the track item */
        emit frameCached(it.first - m_Offset);
    }
}

VOID_NAMESPACE_CLOSE
