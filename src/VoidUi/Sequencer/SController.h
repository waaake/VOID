// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_CONTROLLER_H
#define _SEQUENCER_CONTROLLER_H

/* Qt */
#include <QGraphicsScene>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

class STrack;

class SequencerController
{
public:
    // void BeginDrag(SharedTrackItem* item)
    void SetScene(QGraphicsScene* scene);
    void MoveItem(SharedTrackItem& item, v_frame_t frame);
    // void MoveItem(SharedPlaybackTrack& track, SharedTrackItem& item, v_frame_t frame);
    void MoveItem(SharedTrackItem& item, int currentTrackIndex, int trackIndex, v_frame_t frame);
    STrack* TrackAt(const QPointF& position) const;
    STrack* TrackAt(int index) const;
    STrack* TrackAt(int index);

private:
    QGraphicsScene* m_Scene = { nullptr };
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CONTROLLER_H
