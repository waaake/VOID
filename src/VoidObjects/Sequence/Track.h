// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TRACK_H
#define _VOID_TRACK_H

/* STD */
#include <unordered_set>

/* Qt */
#include <QObject>
#include <QColor>

/* Internal */
#include "Definition.h"
#include "TrackMap.h"
#include "TrackItem.h"
#include "Descriptors.h"
#include "VoidObjects/VoidObject.h"

VOID_NAMESPACE_OPEN

class PlaybackSequence;

class VOID_API PlaybackTrack : public VoidObject
{
    Q_OBJECT

public:
    explicit PlaybackTrack(const Sequence::TrackType& type, QObject* parent = nullptr);
    virtual ~PlaybackTrack();

    const Sequence::TrackType& Type() const { return m_Type; }

    void SetName(const std::string& name) { m_Name = name; }
    void SetName(std::string&& name) { m_Name = std::move(name); }
    const std::string& Name() const { return m_Name; }

    // /* Set a color for the Track */
    // inline void SetColor(const QColor& color)
    // {
    //     /* Update the color for the track */
    //     m_Color = color;
    //     /* Emit a changed signal */
    //     emit updated();
    // }

    std::size_t NumItems() const { return m_Items.Size(); }
    std::size_t ItemIndex(const SharedTrackItem& item) const { return m_Items.ItemIndex(item); }
    std::size_t ItemIndex(const TrackItem* item) const { return m_Items.ItemIndex(item); }
    SharedTrackItem ItemAt(std::size_t index) const { return m_Items.AtIndex(index); }
    MFrameRange ItemRange(std::size_t index) const { return m_Items.AtIndex(index)->TimelineRange(); }
    const std::vector<SharedTrackItem>& Items() const { return m_Items.Items(); }

    Effect* CreateEffect(const SharedTrackItem& item, const std::string& effect);
    int MaxEffects() const { return m_MaxEffects; }

    /**
     * @brief Clears anything in the track and sets the provided media as first
     * 
     * @param media Media to be reset on the track.
     */
    void SetMedia(const SharedMediaClip& media);

    /**
     * Appends the Media to the already existing track of Medis files
     * Which will get played in order
     */
    SharedTrackItem AddMedia(const SharedMediaClip& media);
    SharedMediaClip Media(v_frame_t frame);

    /* Clears the Playback Track */
    void Clear();

    /*
     * Caches all frames of any media on the track
     * emits frameCached for every frame that has been cached
     */
    void Cache(v_frame_t frame);
    void Image(v_frame_t frame, FloatImage& image);
    const FloatImage Image(v_frame_t frame);

    /* Clears all cache from internal media and emits cacheCleared */
    void ClearCache();
    void ClearCache(v_frame_t frame);

    /* Getters */
    inline int StartFrame() const { return m_StartFrame; }
    inline int EndFrame() const { return m_EndFrame; }
    v_frame_t GetSnapFrame(v_frame_t frame, const SharedTrackItem& trackitem, int threshold = 5) const;

    inline bool IsEmpty() const { return m_Items.Empty(); }

    // /* Returns the Color associated with the Track */
    // inline QColor Color() const { return m_Color; }

    /**
     * Describes whether a track is active for playback or taking in elements with menu options
     */
    [[nodiscard]] inline bool Active() const { return m_Visible && m_Enabled; }

    inline bool Visible() const { return m_Visible; }
    
    bool Locked() const { return m_Locked; }
    void Lock(bool lock) { m_Locked = lock; emit updated(); }

    void SetEnabled(bool enable) { m_Enabled = enable; emit updated(); }
    inline bool Enabled() const { return m_Enabled; }

    /**
     * From the track, return the track item which is present at a given frame in the timeline
     * Returns nullptr if there is no trackitem at the given timeframe
     */
    SharedTrackItem GetTrackItem(v_frame_t frame);

    /* The parent of the Track should always be a Sequence, in case it exists inside a Sequence */
    inline PlaybackSequence* Sequence() const { return reinterpret_cast<PlaybackSequence*>(parent()); }
    int TrackIndex() const;

    /* Setters */

    bool RazorAt(v_frame_t frame);
    bool MergeCut(v_frame_t frame);
    bool IsRazored(v_frame_t frame) const { return m_Razored.find(frame) != m_Razored.end(); }

    bool MoveItem(const SharedTrackItem& item, v_frame_t frame);
    bool OffsetItem(const SharedTrackItem& item, int offset);
    bool AddItem(const SharedTrackItem& item);
    bool AddItem(const SharedTrackItem& item, v_frame_t frame);
    // Removes the Track Item at the given frame
    void RemoveItem(v_frame_t frame);
    void RemoveItem(const SharedTrackItem& item);

    /**
     * The track's range is always defined by the track items in it
     * The only thing which can/should be changed of a track is the starting frame
     */
    inline void SetStartFrame(int start) { SetRange(start, start + m_EndFrame); }

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Serialize(std::ostream& out) const override;
    void Deserialize(const rapidjson::Value& in) override;
    void Deserialize(std::istream& in) override;

    const char* TypeName() const override { return "PlaybackTrack"; }

signals: /* Signals Denoting actions in the Track */
    void cleared();
    void itemAdded(const SharedTrackItem&);
    void effectAdded(const Effect* const);
    void itemAboutToBeRemoved(const SharedTrackItem&);
    void maxEffectsChanged();
    void itemRemoved();
    void updated();
    void rangeChanged(int start, int end);

protected: /* Members */
    TrackMap m_Items;
    std::unordered_set<v_frame_t> m_Razored;

    SharedTrackItem m_Recent;
    std::string m_Name;
    int m_StartFrame, m_EndFrame;
    int m_Duration;
    int m_MaxEffects;
    bool m_Visible;
    bool m_Enabled;
    bool m_Locked;
    Sequence::TrackType m_Type;

protected: /* Methods */
    void SetRange(int start, int end, const bool inclusive = true);
    void CalculateMaxEffects(const SharedTrackItem& item);
    void CalculateMaxEffects();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_H
