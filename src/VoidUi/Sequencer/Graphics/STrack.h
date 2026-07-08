// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_GRAPHICS_TRACK_H
#define _SEQUENCER_GRAPHICS_TRACK_H

/* STD */
#include <unordered_map>
#include <vector>

/* Qt */
#include <QGraphicsItem>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/Track.h"

VOID_NAMESPACE_OPEN

class STimelineGeometry;
class STrackItem;

class STrack : public QGraphicsItem
{
public:
    STrack(const SharedPlaybackTrack& track, int index, STimelineGeometry* geometry, QGraphicsItem* parent = nullptr);
    ~STrack();

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void Update();
    void Clear();

    void AddItem(const SharedTrackItem& item);
    void RemoveItem(const SharedTrackItem& item);
    void UpdateItem(const SharedTrackItem& item);

private:
    std::unordered_map<TrackItem*, STrackItem*> m_Items;
    QRectF m_BoundingRect;
    // std::vector<STrackItem*> m_Items;
    SharedPlaybackTrack m_Track;
    STimelineGeometry* m_Geometry;

    int m_Index;

private: /* Methods */
    void BuildItems();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_GRAPHICS_TRACK_H
