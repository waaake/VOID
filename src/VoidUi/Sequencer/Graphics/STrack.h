// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_GRAPHICS_TRACK_H
#define _SEQUENCER_GRAPHICS_TRACK_H

/* STD */
#include <unordered_map>

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"
#include "SRazorItem.h"
#include "VoidObjects/Sequence/Track.h"

VOID_NAMESPACE_OPEN

class STrackItem;

class STrack : public STimelineItem
{
public:
    STrack(const SharedPlaybackTrack& track, SequencerContext* context, QGraphicsItem* parent = nullptr);
    ~STrack();

    SharedPlaybackTrack& Track() { return m_Track; }
    const SharedPlaybackTrack& Track() const { return m_Track; }
    int Index() const { return m_Track->Index(); }
    bool Locked() const { return m_Track->Locked(); }
    bool Enabled() const { return m_Track->Enabled(); }
    bool IsRazored(v_frame_t frame) const { return m_Track->IsRazored(frame); }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void Update() override;
    void Clear();

    void AddEffect(Effect* effect);
    void AddEffect(Effect* effect, int index);
    void AddEffects();
    void RemoveEffect(Effect* effect);
    void AddItem(const SharedTrackItem& item);
    void RemoveItem(const SharedTrackItem& item);
    void UpdateEffects();
    void UpdateItem(const SharedTrackItem& item);
    void UpdateItems();

    STrackItem* ItemAt(int index) const;
    STrackItem* ItemAt(int index);
    STrackItem* Item(const SharedTrackItem& item);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    std::unordered_map<TrackItem*, STrackItem*> m_Items;
    std::unordered_map<Effect*, STimelineEffect*> m_Effects;
    STrackRazorItem* m_RazorMarker;
    SharedPlaybackTrack m_Track;

private: /* Methods */
    void BuildItems();

    friend class STrackItem;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_GRAPHICS_TRACK_H
