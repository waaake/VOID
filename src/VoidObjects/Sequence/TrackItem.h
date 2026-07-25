// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TRACK_ITEM_H
#define _VOID_TRACK_ITEM_H

/* STD */
#include <memory>

/* Qt */
#include <QObject>
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/VoidObject.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration of the Track */
class PlaybackTrack;
class TrackItem;

typedef std::shared_ptr<TrackItem> SharedTrackItem;

class VOID_API TrackItem : public VoidObject
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

    bool Linked() const { return (bool)m_Media; }
    void Unlink();

    /* Getters */
    inline v_frame_t GetOffset() const { return m_Offset; }
    inline SharedMediaClip GetMedia() const { return m_Media; }

    std::string Name() const { return m_Media ? m_Media->Name() : m_Name; }

    /**
     * @brief Updates the Image pointer with the data from the underlying media in the Item.
     * 
     * @param frame Frame number.
     * @param image Image Buffer to be updated.
     */
    void Image(const v_frame_t frame, FloatImage& image);
    const FloatImage Image(v_frame_t frame);

    void ClearCache(v_frame_t frame);

    v_frame_t TimelineIn() const { return m_Start; }
    v_frame_t TimelineOut() const { return m_End; }
    int Duration() const { return m_End - m_Start + 1; }

    // This will change when we have handles implemented
    v_frame_t SourceIn() const { return m_Media ? m_Media->FirstFrame() : 0; }
    v_frame_t SourceOut() const { return m_Media ? m_Media->LastFrame() : 0; }

    // Moves the item to the given frame
    void Move(v_frame_t frame);

    /**
     * Returns whether the given frame is in range of the underlying media
     * Applies the offset of the track item's range back to check against the media
     * 
     * TODO: Consider handle frames when they are implemented.
     */
    inline bool InRange(const v_frame_t frame) const { return m_Media ? m_Media->InRange(frame + m_Offset) : false; }
    bool InTimelineRange(const v_frame_t frame) const { return frame >= m_Start && frame <= m_End; }

    /**
     * Returns the nearest frame of a given frame from the media in TrackItem space
     * This means that the output frame is negated with the offset of the track item
     * E.g. Media's nearest frame for 1003 is 1001 and the offset of the Track Item is 1001
     * Hence for frame 3, the nearest frame is 0 by adding offset and querying media for nearest frame and
     * then negating back the offset
     * 
     * TODO: See if we can improve our logic to get a frame value or Image Data directly?
     */
    inline v_frame_t NearestFrame(const v_frame_t frame) const { return m_Media ? m_Media->NearestFrame(frame + m_Offset) - m_Offset : frame; }

    // /* TODO: Cache the First frame and last frame for Media in that class */
    // inline v_frame_t MediaFirstFrame() const { return m_Media->FirstFrame(); }
    // inline v_frame_t MediaLastFrame() const { return m_Media->LastFrame(); }

    /* The parent of the TrackItem should always be a Track, in case it exists on a Track */
    inline PlaybackTrack* Track() const { return m_Track; }
    void SetTrack(PlaybackTrack* track) { m_Track = track; } 

    inline QColor Color() const { return m_Color; }
    void ResetColor() { if (m_Media) SetColor(m_Media->Color()); }
    void SetColor(const QColor& color);

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Deserialize(const rapidjson::Value& in) override;

    const char* TypeName() const override { return "TrackItem"; }

signals:
    void mediaChanged();
    void updated();
    void rangeChanged(v_frame_t start, v_frame_t end);

    // /**
    //  * Emitted when a frame is cached
    //  * The cache could happen when the media cache operation is run continuously on a thread
    //  * Or if the frame is queried by the viewport
    //  */
    // void frameCached(v_frame_t frame);

protected:
    SharedMediaClip m_Media;
    PlaybackTrack* m_Track;
    std::string m_Name;
    QColor m_Color;
    v_frame_t m_Offset;
    v_frame_t m_Start;
    v_frame_t m_End;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_ITEM_H
