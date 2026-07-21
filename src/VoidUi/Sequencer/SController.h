// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_CONTROLLER_H
#define _SEQUENCER_CONTROLLER_H

/* STD */
#include <unordered_set>

/* Qt */
#include <QObject>
#include <QGraphicsScene>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

class STrack;

class SequencerController : public QObject
{
    Q_OBJECT
public:
    void SetScene(QGraphicsScene* scene);
    void MoveItem(SharedTrackItem& item, v_frame_t frame);
    void MoveItem(SharedTrackItem& item, int currentTrackIndex, int trackIndex, v_frame_t frame);
    void CreateVideoTrack(const SharedPlaybackSequence& sequence);
    void CreateAudioTrack(const SharedPlaybackSequence& seqeunce);

    STrack* TrackAt(const QPointF& position) const;
    STrack* TrackAt(int index) const;
    STrack* TrackAt(int index);

    v_frame_t CurrentFrame() const { return m_Frame; }
    void SetCurrentFrame(v_frame_t frame);
    void RequestFrameChange(v_frame_t frame);
    void SetTrackItemsColor(const std::unordered_set<SharedTrackItem>& items, const QColor& color);
    void SetTrackItemsColor(const std::unordered_set<SharedTrackItem>& items);
    void ToggleTrackLock(const SharedPlaybackTrack& track);
    void ToggleTrackState(const SharedPlaybackTrack& track);

signals:
    void frameChanged(v_frame_t);
    void frameChangeRequested(v_frame_t);

private:
    QGraphicsScene* m_Scene = { nullptr };
    v_frame_t m_Frame;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CONTROLLER_H
