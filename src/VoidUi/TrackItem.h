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

    /* Getters */
    inline int GetOffset() const { return m_Offset; }
    inline Media GetMedia() const { return m_Media; }

    inline int StartFrame() const { return m_StartFrame; }
    inline int EndFrame() const { return m_EndFrame; }

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

protected: /* Members */
    Media m_Media;
    int m_Offset;

    int m_StartFrame;
    int m_EndFrame;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_ITEM_H
