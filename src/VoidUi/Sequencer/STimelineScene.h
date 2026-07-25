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
#include "SContext.h"
#include "SDescriptors.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class SPlayheadItem;
class STrack;

class STimelineScene : public QGraphicsScene
{
    Q_OBJECT
public:
    STimelineScene(SequencerContext* context, QObject* parent = nullptr);

    void AddTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);
    void Clear();

    void AddPlayhead();
    void UpdatePlayhead();
    void Update();
    void UpdateItems();
    STrack* TrackAt(int index) const;
    STrack*& TrackAt(int index);

    void SelectTrackItems(const QRectF& rect);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    std::vector<STrack*> m_Tracks;
    SequencerContext* m_Context;
    SPlayheadItem* m_Playhead;

private:
    int SceneHeight() const;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TIMELINE_SCENE_H
