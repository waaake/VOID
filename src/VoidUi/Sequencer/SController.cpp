// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SController.h"
#include "STimelineScene.h"
#include "VoidCore/Logging.h"
#include "Graphics/STrack.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Commands/SequenceCommands.h"

VOID_NAMESPACE_OPEN

void SequencerController::SetScene(QGraphicsScene* scene)
{
    m_Scene = scene;
}

void SequencerController::SetTimeController(TimelineController* controller)
{
    m_TimelineController = controller;
    connect(m_TimelineController, &TimelineController::timeChanged, this, &SequencerController::SetCurrentFrame, Qt::DirectConnection);
}

void SequencerController::MoveItem(const SharedTrackItem& item, v_frame_t frame)
{
    if (m_EditMode == EditMode::RIPPLE)
        return RippleMoveItem(item, frame);

    // Item was dragged and returned back to the same position
    if (item->TimelineIn() == frame)
        return;

    _MediaBridge.PushCommand(new MoveTrackItemCommand(item, frame));
}

void SequencerController::RippleMoveItem(const SharedTrackItem& item, v_frame_t frame)
{
    if (item->TimelineIn() == frame) return;

    QUndoStack* stack = _MediaBridge.UndoStack();

    stack->beginMacro("Move TrackItem");

    int offset = frame - item->TimelineIn();
    PlaybackTrack* track = item->Track();
    int index = track->ItemIndex(item);

    // We're moving forwards (towards right, so start with the right most item here)
    if (offset > 0)
    {
        for (int i = static_cast<int>(track->ItemCount()) - 1; i > index; --i)
        {
            const SharedTrackItem& trackitem = track->ItemAt(i);
            stack->push(new OffsetItemCommand(trackitem, offset));
        }
    }
    else
    {
        for (int i = index; i < static_cast<int>(track->ItemCount()); ++i)
        {
            const SharedTrackItem& trackitem = track->ItemAt(i);
            stack->push(new OffsetItemCommand(trackitem, offset));
        }    
    }

    stack->push(new MoveTrackItemCommand(item, frame));
    stack->endMacro();
}

void SequencerController::MoveItem(const SharedPlaybackTrack& track, const SharedTrackItem& item, int trackIndex, v_frame_t frame)
{
    if (m_EditMode == EditMode::RIPPLE)
        return RippleMoveItem(track, item, trackIndex, frame);

    _MediaBridge.PushCommand(new MoveItemToTrackCommand(track, item, trackIndex, frame));
}

void SequencerController::RippleMoveItem(const SharedPlaybackTrack& track, const SharedTrackItem& item, int trackIndex, v_frame_t frame)
{
    QUndoStack* stack = _MediaBridge.UndoStack();

    stack->beginMacro("Move TrackItem");

    int index = track->ItemIndex(item);
    stack->push(new MoveItemToTrackCommand(track, item, trackIndex, frame));

    std::size_t max = track->ItemCount();
    // Last Item --- Nothing else to offset/move
    if (index >= max)
        return stack->endMacro();

    // Since we're moving item to a different track, we need to check the item before this index and it's timelineOut to fill in the gap
    SharedTrackItem next = track->ItemAt(index);
    v_frame_t lastFrame = index == 0 ? track->StartFrame() : track->ItemAt(index - 1)->TimelineOut() + 1;
    int offset = lastFrame - next->TimelineIn();

    for (int i = index; i < static_cast<int>(max); ++i)
    {
        const SharedTrackItem& trackitem = track->ItemAt(i);
        stack->push(new OffsetItemCommand(trackitem, offset));
    }

    stack->endMacro();
}

STrack* SequencerController::TrackAt(const QPointF& position) const
{
    if (m_Scene)
    {
        QList<QGraphicsItem*> items = m_Scene->items(position, Qt::IntersectsItemShape, Qt::AscendingOrder);
        for (QGraphicsItem*& item : items)
        {
            if (STrack* track = dynamic_cast<STrack*>(item))
                return track;
        }

    }
    return nullptr;
}

void SequencerController::CreateVideoTrack(const SharedPlaybackSequence& sequence)
{
    _MediaBridge.PushCommand(new CreateTrackCommand(sequence, Sequence::TrackType::VIDEO));
}

void SequencerController::CreateAudioTrack(const SharedPlaybackSequence& sequence)
{
    _MediaBridge.PushCommand(new CreateTrackCommand(sequence, Sequence::TrackType::AUDIO));
}

void SequencerController::RemoveTracks(const SharedPlaybackSequence& sequence, const std::unordered_set<SharedPlaybackTrack>& tracks)
{
    QUndoStack* stack = _MediaBridge.UndoStack();

    stack->beginMacro("Remove Track(s)");
    for (const auto& track : tracks)
        stack->push(new DeleteTrackCommand(sequence, track->TrackIndex(), track->Type()));
    stack->endMacro();
}

void SequencerController::RemoveTrackItems(const SharedPlaybackSequence& sequence, const std::unordered_set<SharedTrackItem>& items)
{
    if (m_EditMode == EditMode::RIPPLE)
        return RippleRemoveTrackItems(sequence, items);

    QUndoStack* stack = _MediaBridge.UndoStack();

    // std::vector<SharedTrackItem> trackitems;
    // trackitems.reserve(items.size());

    // for (const auto& item : items)
    //     trackitems.push_back(item);

    // std::sort(trackitems.begin(), trackitems.end(), [&](const SharedTrackItem& _a, const SharedTrackItem& _b) -> bool
    // {
    //     PlaybackTrack* _atrack = _a->Track();
    //     PlaybackTrack* _btrack = _b->Track();

    //     int _atrackidx = _atrack->Type() == Sequence::TrackType::VIDEO ? sequence->VideoTrackIndex(_atrack) : sequence->AudioTrackIndex(_atrack);
    //     int _btrackidx = _btrack->Type() == Sequence::TrackType::VIDEO ? sequence->VideoTrackIndex(_btrack) : sequence->AudioTrackIndex(_btrack);

    //     // Sort ascending based on the track index -- item _b belongs to a different track than _a
    //     if (_atrackidx != _btrackidx)
    //         return _atrackidx < _btrackidx;

    //     // Sort descending based on the track item index
    //     // we need the items to be reversed such that when deleting them, the index of other items don't change
    //     // and running undo works as expected
    //     return _atrack->ItemIndex(_a) > _btrack->ItemIndex(_b);
    // });

    stack->beginMacro("Remove TrackItem(s)");
    for (const auto& item : items)
    {
        const PlaybackTrack* track = item->Track();
        stack->push(new DeleteTrackItemCommand(sequence, track->Type(), track->TrackIndex(), track->ItemIndex(item)));
    }
    stack->endMacro();
}

void SequencerController::RippleRemoveTrackItems(const SharedPlaybackSequence& sequence, const std::unordered_set<SharedTrackItem>& items)
{
    QUndoStack* stack = _MediaBridge.UndoStack();

    // std::vector<SharedTrackItem> trackitems;
    // trackitems.reserve(items.size());

    // for (const auto& item : items)
    //     trackitems.push_back(item);

    // std::sort(trackitems.begin(), trackitems.end(), [&](const SharedTrackItem& _a, const SharedTrackItem& _b) -> bool
    // {
    //     PlaybackTrack* _atrack = _a->Track();
    //     PlaybackTrack* _btrack = _b->Track();

    //     int _atrackidx = _atrack->Type() == Sequence::TrackType::VIDEO ? sequence->VideoTrackIndex(_atrack) : sequence->AudioTrackIndex(_atrack);
    //     int _btrackidx = _btrack->Type() == Sequence::TrackType::VIDEO ? sequence->VideoTrackIndex(_btrack) : sequence->AudioTrackIndex(_btrack);

    //     // Sort ascending based on the track index -- item _b belongs to a different track than _a
    //     if (_atrackidx != _btrackidx)
    //         return _atrackidx < _btrackidx;

    //     // Sort descending based on the track item index
    //     // we need the items to be reversed such that when deleting them, the index of other items don't change
    //     // and running undo works as expected
    //     return _atrack->ItemIndex(_a) > _btrack->ItemIndex(_b);
    // });

    stack->beginMacro("Remove TrackItem(s)");
    for (const auto& item : items)
    {
        const PlaybackTrack* track = item->Track();
        int index = track->ItemIndex(item);

        stack->push(new DeleteTrackItemCommand(sequence, track->Type(), track->TrackIndex(), track->ItemIndex(item)));

        std::size_t max = track->ItemCount();
        // Last Item --- Nothing else to offset/move
        if (index >= max)
            continue;

        SharedTrackItem next = track->ItemAt(index);
        v_frame_t lastFrame = index == 0 ? track->StartFrame() : track->ItemAt(index - 1)->TimelineOut() + 1;
        int offset = lastFrame - next->TimelineIn();

        for (int i = index; i < static_cast<int>(max); ++i)
        {
            const SharedTrackItem& trackitem = track->ItemAt(i);
            stack->push(new OffsetItemCommand(trackitem, offset));
        }
    }
    stack->endMacro();
}

STrack* SequencerController::TrackAt(int index) const
{
    if (STimelineScene* scene = dynamic_cast<STimelineScene*>(m_Scene))
        return scene->TrackAt(index);
    return nullptr;
}

STrack* SequencerController::TrackAt(int index)
{
    if (STimelineScene* scene = dynamic_cast<STimelineScene*>(m_Scene))
        return scene->TrackAt(index);
    return nullptr;
}

void SequencerController::SetCurrentFrame(v_frame_t frame)
{
    if (m_Frame == frame)
        return;

    m_Frame = frame;
    emit frameChanged(frame);
}

void SequencerController::RequestFrameChange(v_frame_t frame)
{
    m_TimelineController->SetFrame(frame);
    SetCurrentFrame(frame);
}

void SequencerController::SetTrackItemsColor(const std::unordered_set<SharedTrackItem>& items, const QColor& color)
{
    QUndoStack* stack = _MediaBridge.UndoStack();

    stack->beginMacro("Set Trackitem(s) color");
    for (auto& item : items)
        stack->push(new SetTrackItemColorCommand(item, color));
    stack->endMacro();
}

void SequencerController::SetTrackItemsColor(const std::unordered_set<SharedTrackItem>& items)
{
    QUndoStack* stack = _MediaBridge.UndoStack();

    stack->beginMacro("Reset Trackitem(s) color");
    for (auto& item : items)
        stack->push(new SetTrackItemColorCommand(item));
    stack->endMacro();
}

void SequencerController::ToggleTrackLock(const SharedPlaybackTrack& track)
{
    _MediaBridge.PushCommand(new ToggleLockTrackCommand(track));
}

void SequencerController::ToggleTrackState(const SharedPlaybackTrack& track)
{
    _MediaBridge.PushCommand(new ToggleTrackStateCommand(track));
}

void SequencerController::RazorAt(const SharedPlaybackSequence& sequence, v_frame_t frame)
{
    _MediaBridge.PushCommand(new RazorSequenceCommand(sequence, frame));
}

void SequencerController::RazorAt(const SharedPlaybackTrack& track, v_frame_t frame)
{
    _MediaBridge.PushCommand(new RazorTrackCommand(track, frame));
}

void SequencerController::MergeCut(const SharedPlaybackTrack& track, v_frame_t frame)
{
    _MediaBridge.PushCommand(new MergeCutCommand(track, frame));
}

void SequencerController::OffsetItemSource(const SharedTrackItem& item, int offset)
{
    _MediaBridge.PushCommand(new OffsetItemSourceCommand(item, offset));
}

void SequencerController::TrimItemHead(const SharedTrackItem& item, int handle)
{
    _MediaBridge.PushCommand(new TrimItemHeadCommand(item, handle));
}

void SequencerController::TrimItemTail(const SharedTrackItem& item, int handle)
{
    if (m_EditMode == EditMode::RIPPLE) return RippleTrimItemTail(item, handle);
    _MediaBridge.PushCommand(new TrimItemTailCommand(item, handle));
}

void SequencerController::RippleTrimItemTail(const SharedTrackItem& item, int handle)
{
    QUndoStack* stack = _MediaBridge.UndoStack();

    stack->beginMacro("Trim Item");
    stack->push(new TrimItemTailCommand(item, handle));

    PlaybackTrack* track = item->Track();
    int index = track->ItemIndex(item);

    std::size_t max = track->ItemCount();
    // Last Item --- Nothing else to offset/move
    if (index + 1 >= max)
        return stack->endMacro();

    SharedTrackItem next = track->ItemAt(index + 1);
    v_frame_t lastFrame = item->TimelineOut() + 1;
    int offset = lastFrame - next->TimelineIn();

    for (int i = index + 1; i < static_cast<int>(max); ++i)
    {
        const SharedTrackItem& trackitem = track->ItemAt(i);
        stack->push(new OffsetItemCommand(trackitem, offset));
    }

    stack->endMacro();
}

VOID_NAMESPACE_CLOSE
