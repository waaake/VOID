// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_CONTROLLER_H
#define _SEQUENCER_CONTROLLER_H

/* STD */
#include <unordered_set>

/* Qt */
#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsScene>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/TrackItem.h"
#include "VoidUi/Timeline/TimelineController.h"

VOID_NAMESPACE_OPEN

class STrack;

class SequencerController : public QObject
{
    Q_OBJECT
public:
    enum class EditMode
    {
        NO_OVERWRITE,
        OVERWRITE,
        RIPPLE
    };

public:
    void SetScene(QGraphicsScene* scene);
    QGraphicsScene* Scene() const { return m_Scene; }

    void SetTimeController(TimelineController* controller);
    TimelineController* TimeController() { return m_TimelineController; }

    void AddToScene(QGraphicsItem* item) { m_Scene->addItem(item); }
    void RemoveFromScene(QGraphicsItem* item) { m_Scene->removeItem(item); }
    void MoveItem(const SharedTrackItem& item, v_frame_t frame);
    void RippleMoveItem(const SharedTrackItem& item, v_frame_t frame);
    void MoveItem(const SharedPlaybackTrack& track, const SharedTrackItem& item, int trackIndex, v_frame_t frame);
    void RippleMoveItem(const SharedPlaybackTrack& track, const SharedTrackItem& item, int trackIndex, v_frame_t frame);
    void CreateVideoTrack(const SharedPlaybackSequence& sequence);
    void CreateAudioTrack(const SharedPlaybackSequence& sequence);
    void RemoveTracks(const SharedPlaybackSequence& sequence, const std::unordered_set<SharedPlaybackTrack>& tracks);
    void RemoveTrackItems(const SharedPlaybackSequence& sequence, const std::unordered_set<SharedTrackItem>& items);
    void RippleRemoveTrackItems(const SharedPlaybackSequence& sequence, const std::unordered_set<SharedTrackItem>& items);

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
    void ToggleItemState(const std::unordered_set<SharedTrackItem>& items);
    void ToggleItemState(const std::unordered_set<Effect*>& effects);

    void RazorAt(const SharedPlaybackSequence& sequence, v_frame_t frame);
    void RazorAt(const SharedPlaybackTrack& track, v_frame_t frame);
    void MergeCut(const SharedPlaybackTrack& track, v_frame_t frame);
    void OffsetItemSource(const SharedTrackItem& item, int offset);

    void TrimItemHead(const SharedTrackItem& item, int handle);
    void TrimItemTail(const SharedTrackItem& item, int handle);
    void RippleTrimItemTail(const SharedTrackItem& item, int handle);

    void SetEditMode(const EditMode& mode) { m_EditMode = mode; }
    const EditMode& GetEditMode() const { return m_EditMode; }

    void EditEffect(Effect* effect) { emit editEffectRequested(effect); }

signals:
    void frameChanged(v_frame_t);
    void editEffectRequested(Effect*);

private:
    QGraphicsScene* m_Scene = { nullptr };
    TimelineController* m_TimelineController = { nullptr };
    v_frame_t m_Frame;
    EditMode m_EditMode = { EditMode::NO_OVERWRITE };
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CONTROLLER_H
