// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TRACK_ITEM_H
#define _VOID_TRACK_ITEM_H

/* STD */
#include <memory>
#include <vector>

/* Qt */
#include <QObject>
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/Frame.h"
#include "VoidObjects/VoidObject.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration of the Track */
class PlaybackTrack;
class TrackItem;
class Effect;

typedef std::shared_ptr<TrackItem> SharedTrackItem;

class VOID_API TrackItem : public VoidObject
{
    Q_OBJECT
public:
    TrackItem(QObject* parent = nullptr);
    TrackItem(const SharedMediaClip& media, v_frame_t start, v_frame_t end, v_frame_t offset = 0, QObject* parent = nullptr);

    virtual ~TrackItem();

    TrackItem(const TrackItem& other);
    TrackItem& operator=(const TrackItem& other);

    TrackItem(TrackItem&& other) noexcept;
    TrackItem& operator=(TrackItem&& other) noexcept;

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

    std::size_t Index() const;

    void SetEnabled(bool enable);
    bool Enabled() const { return m_Enabled; }

    QPixmap Thumbnail() const { return m_Media ? m_Media->Thumbnail() : ProjectEntity::DefaultThumbnail(); }

    /* Getters */
    inline v_frame_t GetOffset() const { return m_Offset; }
    inline SharedMediaClip GetMedia() const { return m_Media; }
    SequenceFrame InternalFrame(v_frame_t frame);

    std::string Name() const { return m_Media ? m_Media->Name() : m_Name; }
    Effect* CreateEffect(const std::string& type);
    Effect* CreateEffect(const std::string& type, const std::string& name);
    void AddEffect(Effect* effect);
    void InsertEffect(Effect* effect, int index);
    bool RemoveEffect(const std::string& name);
    void RemoveEffect(int index, bool destroy = true);
    void ClearEffects();
    bool HasEffects() const { return !m_Effects.empty(); }
    int NumEffects() const { return static_cast<int>(m_Effects.size()); }
    int EffectIndex(const Effect* const effect) const;
    Effect* EffectAt(int index) const { return m_Effects.at(index); }
    const std::vector<Effect*>& Effects() const { return m_Effects; }

    /**
     * @brief Updates the Image pointer with the data from the underlying media in the Item.
     *
     * @param frame Frame number.
     * @param image Image Buffer to be updated.
     */
    void Image(const v_frame_t frame, FloatImage& image);
    const FloatImage Image(v_frame_t frame);

    void ClearCache(v_frame_t frame);

    /**
     *              Timeline In         Timeline Out
     *                    v              v
     *  |' ' ' ' ' ' ' '  """""""""""""""" ' ' ' ' ' ' ' '|
     *  |                 |              |                |
     *  |                 | <-Duration-> |                |
     *  | <----Head-----> |              | <---Tail------>|
     *  |_ _ _ _ _ _ _ _ _|______________| _ _ _ _ _ _ _ _|
     *  ^                 ^              ^                ^
     * Source start   Source In       Source Out       Source end
     */

    v_frame_t TimelineIn() const { return m_TimelineIn; }
    v_frame_t TimelineOut() const { return m_TimelineOut; }
    MFrameRange TimelineRange() const { return MFrameRange(m_TimelineIn, m_TimelineOut); }

    void SetTimelineIn(v_frame_t frame);
    void SetTimelineOut(v_frame_t frame);

    int Duration() const { return m_TimelineOut - m_TimelineIn + 1; }

    v_frame_t SourceIn() const { return m_SourceIn; }
    v_frame_t SourceOut() const { return m_SourceOut; }

    void SetSourceIn(v_frame_t frame);
    void SetSourceOut(v_frame_t frame);

    v_frame_t HeadHandle() const { return m_SourceIn - m_Media->FirstFrame(); }
    v_frame_t TailHandle() const { return m_Media->LastFrame() - (m_SourceIn + (m_TimelineOut - m_TimelineIn)); }

    void TrimHead(int handle);
    void TrimTail(int handle);

    // Moves the item to the given frame
    void Move(v_frame_t frame);
    void Offset(int offset) { Move(m_TimelineIn + offset); }

    /**
     * Returns whether the given frame is in range of the underlying media
     * Applies the offset of the track item's range back to check against the media
     *
     * TODO: Consider handle frames when they are implemented.
     */
    inline bool InRange(const v_frame_t frame) const { return m_Media ? m_Media->InRange(frame + m_Offset) : false; }
    bool InTimelineRange(const v_frame_t frame) const { return frame >= m_TimelineIn && frame <= m_TimelineOut; }

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
    Core::Project* Project() const;

    inline QColor Color() const { return m_Color; }
    void ResetColor() { if (m_Media) SetColor(m_Media->Color()); }
    void SetColor(const QColor& color);

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Serialize(std::ostream& out) const override;
    void Deserialize(const rapidjson::Value& in) override;
    void Deserialize(std::istream& in) override;
    
    const char* TypeName() const override { return "TrackItem"; }

signals:
    void mediaChanged();
    void updated();
    void rangeChanged(const MFrameRange& current, const MFrameRange& previous);
    void effectCreated(Effect*);
    void effectAboutToBeRemoved(Effect*);
    void stateChanged();

protected:
    std::vector<Effect*> m_Effects;
    SharedMediaClip m_Media;
    PlaybackTrack* m_Track;
    std::string m_Name;
    QColor m_Color;

    v_frame_t m_Offset;

    v_frame_t m_TimelineIn;
    v_frame_t m_TimelineOut;

    v_frame_t m_SourceIn;
    v_frame_t m_SourceOut;

    bool m_Enabled;

private:
    void ResetEffectsRange(const MFrameRange& updated);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_ITEM_H
