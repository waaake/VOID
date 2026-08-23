// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TIMELINE_SCENE_H
#define _SEQUENCER_TIMELINE_SCENE_H

/* STD */
#include <vector>

/* Qt */
#include <QGraphicsScene>

/* Internal */
#include "Definition.h"
#include "Descriptors.h"
#include "SContext.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class SPlayheadItem;
class SRazorItem;
class STrack;
class SPreviewTrackItem;

class STimelineScene : public QGraphicsScene
{
    Q_OBJECT
public:
    STimelineScene(SequencerContext* context, QObject* parent = nullptr);

    void AddTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);
    void Clear();

    void AddRazorhead();
    void SetRazorX(int x);
    void ToggleRazorhead(bool visible);
    void AddPlayhead();
    void UpdatePlayhead();
    void Update();
    void UpdateItems();
    STrack* TrackAt(int index) const;
    STrack*& TrackAt(int index);

    void SelectItems(const QRectF& rect);
    void InitDraggableItems(const std::vector<SharedMediaClip>& media);
    void MoveDraggableItems(const QPointF& position);
    void DestroyDraggableItems();
    void DropItems(const QPointF& postion);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    std::vector<STrack*> m_Tracks;
    SequencerContext* m_Context;
    SPlayheadItem* m_Playhead;
    SRazorItem* m_Razorhead;

    // DragItems
    std::vector<SPreviewTrackItem*> m_DraggedItems;

private:
    int SceneHeight() const;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TIMELINE_SCENE_H
