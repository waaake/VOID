// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SController.h"
#include "STimelineScene.h"
#include "VoidCore/Logging.h"
#include "Graphics/STrack.h"
// #include "VoidObjects/Sequence/Track.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Commands/SequenceCommands.h"

VOID_NAMESPACE_OPEN

void SequencerController::SetScene(QGraphicsScene* scene)
{
    m_Scene = scene;
}

void SequencerController::MoveItem(SharedTrackItem& item, v_frame_t frame)
{
    // Item was dragged and returned back to the same position
    if (item->TimelineIn() == frame)
        return;

    _MediaBridge.PushCommand(new MoveTrackItemCommand(item, frame));
}

void SequencerController::MoveItem(SharedTrackItem& item, int currentTrackIndex, int trackIndex, v_frame_t frame)
{
    _MediaBridge.PushCommand(new MoveItemToTrackCommand(this, item, currentTrackIndex, trackIndex, frame));
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
    // sequence->CreateTrack(Sequence::TrackType::VIDEO);
    _MediaBridge.PushCommand(new CreateTrackCommand(sequence, Sequence::TrackType::VIDEO));
}

void SequencerController::CreateAudioTrack(const SharedPlaybackSequence& sequence)
{
    // sequence->CreateTrack(Sequence::TrackType::AUDIO);
    _MediaBridge.PushCommand(new CreateTrackCommand(sequence, Sequence::TrackType::AUDIO));
}

void SequencerController::RemoveTracks(const SharedPlaybackSequence& sequence, const std::unordered_set<SharedPlaybackTrack>& tracks)
{
    Project* project = _MediaBridge.ActiveProject();
    QUndoStack* stack = project->UndoStack();

    stack->beginMacro("Remove Track(s)");
    for (const auto& track : tracks)
        stack->push(new DeleteTrackCommand(sequence, track->TrackIndex(), track->Type()));
    stack->endMacro();
}

void SequencerController::RemoveTrackItems(const SharedPlaybackSequence& sequence, const std::unordered_set<SharedTrackItem>& items)
{
    Project* project = _MediaBridge.ActiveProject();
    QUndoStack* stack = project->UndoStack();

    stack->beginMacro("Remove TrackItem(s)");
    for (const auto& item : items)
    {
        const PlaybackTrack* track = item->Track();
        stack->push(new DeleteTrackItemCommand(sequence, track->Type(), track->TrackIndex(), track->ItemIndex(item)));
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
    emit frameChangeRequested(frame);
    SetCurrentFrame(frame);
}

void SequencerController::SetTrackItemsColor(const std::unordered_set<SharedTrackItem>& items, const QColor& color)
{
    Project* project = _MediaBridge.ActiveProject();
    QUndoStack* stack = project->UndoStack();

    stack->beginMacro("Set Trackitem(s) color");
    for (auto& item : items)
        stack->push(new SetTrackItemColorCommand(item, color));
    stack->endMacro();
}

void SequencerController::SetTrackItemsColor(const std::unordered_set<SharedTrackItem>& items)
{
    Project* project = _MediaBridge.ActiveProject();
    QUndoStack* stack = project->UndoStack();

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

VOID_NAMESPACE_CLOSE
