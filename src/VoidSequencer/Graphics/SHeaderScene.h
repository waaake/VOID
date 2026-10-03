// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_HEADER_SCENE_H
#define _SEQUENCER_HEADER_SCENE_H

/* STD */
#include <vector>

/* Qt */
#include <QGraphicsScene>

/* Internal */
#include "Definition.h"
#include "VoidSequencer/Descriptors.h"
#include "VoidSequencer/SContext.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class STrackHeaderItem;

class SHeaderScene : public QGraphicsScene
{
public:
    SHeaderScene(SequencerContext* context, QObject* parent = nullptr);
    ~SHeaderScene();

    void AddVideoTrack(const SharedPlaybackTrack& track);
    void AddAudioTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);
    void Clear();

    void Update();
    void UpdateItems();
    STrackHeaderItem* TrackAt(int index) const;
    STrackHeaderItem*& TrackAt(int index);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    std::vector<STrackHeaderItem*> m_VTracks;
    std::vector<STrackHeaderItem*> m_ATracks;
    SequencerContext* m_Context;

private:
    void RemoveVideoTrack(const SharedPlaybackTrack& track);
    void RemoveAudioTrack(const SharedPlaybackTrack& track);
    void ResizeScene();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_HEADER_SCENE_H
