#ifndef _VOID_TRACK_ITEM_H
#define _VOID_TRACK_ITEM_H

/* STD */
#include <memory>

/* Qt */
#include <QObject>
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidObject.h"
#include "VoidUi/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration of the Track */
class PlaybackTrack;
class TrackItem;

typedef std::shared_ptr<TrackItem> SharedTrackItem;

class TrackItem : public VoidObject
{
    Q_OBJECT

public:
    TrackItem(QObject* parent = nullptr);
    TrackItem(const SharedMediaClip& media, v_frame_t start, v_frame_t end, v_frame_t offset = 0, QObject* parent = nullptr);

    virtual ~TrackItem();

    /**
     * Update the media on the track item
     * Offset corresponds to the offet in the framerange as when compared against the original range
     * of the media. 
     * This is the number which when added to the range of the TrackItem gives the range of the Media
     * 
     * e.g. Media Range: 1001 - 1010
     * TrackItem Range: 1 - 10
     * Offset: 1000
     */
    void SetMedia(const SharedMediaClip& media, v_frame_t offset = 0);

    void SetRange(v_frame_t start, v_frame_t end);

    inline void SetColor(const QColor& color) { m_Media->SetColor(color); }

    /** 
     * Caches media frames
     * emits frameCached for each of them emitted
     */
    void Cache();

    /* Getters */
    inline v_frame_t GetOffset() const { return m_Offset; }
    inline SharedMediaClip GetMedia() const { return m_Media; }

    /**
     * Retrieves the image pointer from the media for a frame which has to be offsetted by the current offset
     */
    SharedPixels GetImage(const v_frame_t frame);

    inline v_frame_t StartFrame() const { return m_StartFrame; }
    inline v_frame_t EndFrame() const { return m_EndFrame; }

    /**
     * Returns whether the given frame is in range of the underlying media
     * Applies the offset of the track item's range back to check against the media
     * 
     * TODO: Consider handle frames when they are implemented.
     */
    inline bool HasFrame(const v_frame_t frame) const { return m_Media->HasFrame(frame + m_Offset); }

    /**
     * Returns the nearest frame of a given frame from the media in TrackItem space
     * This means that the output frame is negated with the offset of the track item
     * E.g. Media's nearest frame for 1003 is 1001 and the offset of the Track Item is 1001
     * Hence for frame 3, the nearest frame is 0 by adding offset and querying media for nearest frame and
     * then negating back the offset
     * 
     * TODO: See if we can improve our logic to get a frame value or Image Data directly?
     */
    inline v_frame_t NearestFrame(const v_frame_t frame) const { return m_Media->NearestFrame(frame + m_Offset) - m_Offset; }

    /* TODO: Cache the First frame and last frame for Media in that class */
    inline v_frame_t MediaFirstFrame() const { return m_Media->FirstFrame(); }
    inline v_frame_t MediaLastFrame() const { return m_Media->LastFrame(); }

    /* The parent of the TrackItem should always be a Track, in case it exists on a Track */
    inline PlaybackTrack* Track() const { return reinterpret_cast<PlaybackTrack*>(parent()); }

    inline QColor Color() const { return m_Media->Color(); }

signals: /* Signals denoting Actions in the TrackItem */
    void mediaChanged();

    /* This signal denotes that something in the track item was changed/modified i.e. updated */
    void updated();

    /**
     * Emitted when the time range of the track item has changed
     * includes the start and end frame of the track item
     */
    void rangeChanged(v_frame_t start, v_frame_t end);

    /**
     * Emitted when a frame is cached
     * The cache could happen when the media cache operation is run continuously on a thread
     * Or if the frame is queried by the viewport
     */
    void frameCached(v_frame_t frame);

protected: /* Members */
    SharedMediaClip m_Media;
    v_frame_t m_Offset;

    v_frame_t m_StartFrame;
    v_frame_t m_EndFrame;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_ITEM_H
