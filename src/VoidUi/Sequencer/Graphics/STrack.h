// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_GRAPHICS_TRACK_H
#define _SEQUENCER_GRAPHICS_TRACK_H

/* STD */
#include <unordered_map>

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"
#include "VoidObjects/Sequence/Track.h"

VOID_NAMESPACE_OPEN

class STrackItem;

class STrack : public STimelineItem
{
public:
    STrack(const SharedPlaybackTrack& track, int index, SequencerContext* context, QGraphicsItem* parent = nullptr);
    ~STrack();

    SharedPlaybackTrack& Track() { return m_Track; }
    const SharedPlaybackTrack& Track() const { return m_Track; }
    int Index() const { return m_Index; }
    bool Locked() const { return m_Track->Locked(); }
    bool Enabled() const { return m_Track->Enabled(); }
    // int ItemCount() const { return m_Track->ItemCount(); }
    // STrackItem* BuildItem(int index);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void Update() override;
    void Clear();

    void AddItem(const SharedTrackItem& item);
    void RemoveItem(const SharedTrackItem& item);
    void UpdateItem(const SharedTrackItem& item);
    void UpdateItems();

    STrackItem* ItemAt(int index) const;
    STrackItem* ItemAt(int index);
    STrackItem* Item(const SharedTrackItem& item);

private:
    std::unordered_map<TrackItem*, STrackItem*> m_Items;
    SharedPlaybackTrack m_Track;

    int m_Index;

private: /* Methods */
    void BuildItems();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_GRAPHICS_TRACK_H
