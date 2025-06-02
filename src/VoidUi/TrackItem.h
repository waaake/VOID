#ifndef _VOID_TRACK_ITEM_H
#define _VOID_TRACK_ITEM_H

/* STD */
#include <memory>

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidCore/Media.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration of the Track */
class PlaybackTrack;
class TrackItem;

typedef std::shared_ptr<TrackItem> SharedTrackItem;

class TrackItem : public QObject
{
    Q_OBJECT

public:
    TrackItem(QObject* parent = nullptr);
    TrackItem(const Media& media, int start, int end, int offset = 0, QObject* parent = nullptr);

    virtual ~TrackItem();

    /*
     * Update the media on the track item
     * Offset corresponds to the offet in the framerange as when compared against the original range
     * of the media. 
     * This is the number which when added to the range of the TrackItem gives the range of the Media
     * 
     * e.g. Media Range: 1001 - 1010
     * TrackItem Range: 1 - 10
     * Offset: 1000
     */
    void SetMedia(const Media& media, int offset = 0);

    void SetRange(int start, int end);

    /* 
     * Caches media frames
     * emits frameCached for each of them emitted
     */
    void Cache();

    /* Getters */
    inline int GetOffset() const { return m_Offset; }
    inline Media GetMedia() const { return m_Media; }

    /*
     * Retrieves the image pointer from the media for a frame which has to be offsetted by the current offset
     */
    VoidImageData* GetImage(const int frame);

    inline int StartFrame() const { return m_StartFrame; }
    inline int EndFrame() const { return m_EndFrame; }

    /*
     * Returns the nearest frame of a given frame from the media in TrackItem space
     * This means that the output frame is negated with the offset of the track item
     * E.g. Media's nearest frame for 1003 is 1001 and the offset of the Track Item is 1001
     * Hence for frame 3, the nearest frame is 0 by adding offset and querying media for nearest frame and
     * then negating back the offset
     * 
     * TODO: See if we can improve our logic to get a frame value or Image Data directly?
     */
    inline int NearestFrame(const int frame) const { return m_Media.NearestFrame(frame + m_Offset) - m_Offset; }

    /* TODO: Cache the First frame and last frame for Media in that class */
    inline int MediaFirstFrame() const { return m_Media.FirstFrame(); }
    inline int MediaLastFrame() const { return m_Media.LastFrame(); }

    /* The parent of the TrackItem should always be a Track, in case it exists on a Track */
    inline PlaybackTrack* Track() const { return reinterpret_cast<PlaybackTrack*>(parent()); }

signals: /* Signals denoting Actions in the TrackItem */
    void mediaChanged();

    /* This signal denotes that something in the track item was changed/modified i.e. updated */
    void updated();

    /*
     * Emitted when the time range of the track item has changed
     * includes the start and end frame of the track item
     */
    void rangeChanged(int start, int end);

    /*
     * Emitted when a frame is cached
     * The cache could happen when the media cache operation is run continuously on a thread
     * Or if the frame is queried by the viewport
     */
    void frameCached(int frame);

protected: /* Members */
    Media m_Media;
    int m_Offset;

    int m_StartFrame;
    int m_EndFrame;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_ITEM_H
