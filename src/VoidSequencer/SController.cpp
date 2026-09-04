// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SController.h"
#include "STimelineScene.h"
#include "VoidCore/Logging.h"
#include "Graphics/STrack.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"
#include "Commands/SequenceCommands.h"

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

void SequencerController::Cut(const std::unordered_set<SharedTrackItem>& items)
{
    m_TrackItemClipboard.items = items;
    m_TrackItemClipboard.context = ClipboardContext::CUT;
}

void SequencerController::Cut(const std::unordered_set<SharedPlaybackTrack>& tracks)
{
    m_TrackClipboard.items = tracks;
    m_TrackClipboard.context = ClipboardContext::CUT;
}

void SequencerController::Copy(const std::unordered_set<SharedTrackItem>& items)
{
    m_TrackItemClipboard.items = items;
    m_TrackItemClipboard.context = ClipboardContext::COPY;
}

void SequencerController::Copy(const std::unordered_set<SharedPlaybackTrack>& tracks)
{
    m_TrackClipboard.items = tracks;
    m_TrackClipboard.context = ClipboardContext::COPY;
}

void SequencerController::Paste(Sequence::Context&& context)
{
    if (m_TrackItemClipboard)
    {
        if (m_TrackItemClipboard.context == ClipboardContext::COPY)
        {
            QUndoStack* stack = _MediaBridge.UndoStack();
            stack->beginMacro("Copy TrackItem(s)");

            for (const auto& item : m_TrackItemClipboard.items)
            {
                stack->push(new CopyPasteTrackItemCommand(Sequence::Context::Get(item), context));
                context.frame += item->Duration();
            }

            stack->endMacro();
        }
        else if (m_TrackItemClipboard.context == ClipboardContext::CUT)
        {
            QUndoStack* stack = _MediaBridge.UndoStack();
            stack->beginMacro("Cut TrackItem(s)");

            for (const auto& item : m_TrackItemClipboard.items)
            {
                stack->push(new CutPasteTrackItemCommand(Sequence::Context::Get(item), context));
                context.frame += item->Duration();
            }

            stack->endMacro();
            // Cut-paste can happen only once
            m_TrackItemClipboard.Reset();
        }
    }
    else if (m_TrackClipboard)
    {
        if (m_TrackClipboard.context == ClipboardContext::COPY)
        {
            QUndoStack* stack = _MediaBridge.UndoStack();
            stack->beginMacro("Copy Track(s)");

            for (const auto& track : m_TrackClipboard.items)
                stack->push(new CopyPasteTrackCommand(Sequence::Context::Get(track), context));

            stack->endMacro();
        }
        else if (m_TrackClipboard.context == ClipboardContext::CUT)
        {
            QUndoStack* stack = _MediaBridge.UndoStack();
            stack->beginMacro("Cut Track(s)");

            for (const auto& track : m_TrackClipboard.items)
                stack->push(new CutPasteTrackCommand(Sequence::Context::Get(track), context));
            
            stack->endMacro();
        }
    }
}

void SequencerController::CreateTrackItems(const std::vector<std::pair<const SharedMediaClip, v_frame_t>>& media, const SharedPlaybackTrack& track)
{
    QUndoStack* stack = _MediaBridge.UndoStack();
    stack->beginMacro("Add media to track");

    for (const std::pair<const SharedMediaClip, v_frame_t>& entity : media)
        stack->push(new CreateTrackItemCommand(entity.first, track, entity.second));

    stack->endMacro();
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
        for (int i = static_cast<int>(track->NumItems()) - 1; i > index; --i)
        {
            const SharedTrackItem& trackitem = track->ItemAt(i);
            stack->push(new OffsetItemCommand(trackitem, offset));
        }
    }
    else
    {
        for (int i = index; i < static_cast<int>(track->NumItems()); ++i)
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

    std::size_t max = track->NumItems();
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

void SequencerController::RemoveTracks(const std::unordered_set<SharedPlaybackTrack>& tracks)
{
    QUndoStack* stack = _MediaBridge.UndoStack();

    stack->beginMacro("Remove Track(s)");
    for (const auto& track : tracks)
        stack->push(new DeleteTrackCommand(track));
    stack->endMacro();
}

void SequencerController::RemoveTrackItems(const std::unordered_set<SharedTrackItem>& items)
{
    if (m_EditMode == EditMode::RIPPLE)
        return RippleRemoveTrackItems(items);

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
        stack->push(new DeleteTrackItemCommand(item));
    }
    stack->endMacro();
}

void SequencerController::RippleRemoveTrackItems(const std::unordered_set<SharedTrackItem>& items)
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

        stack->push(new DeleteTrackItemCommand(item));

        std::size_t max = track->NumItems();
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

void SequencerController::CreateEffect(const std::unordered_set<SharedPlaybackTrack>& tracks, const std::string& type)
{
    QUndoStack* stack = _MediaBridge.UndoStack();

    QString text("Create '%1' Effect");
    stack->beginMacro(text.arg(type.c_str()));

    for (const SharedPlaybackTrack& track : tracks)
        stack->push(new CreateTrackEffectCommand(track, type));

    stack->endMacro();
}

void SequencerController::CreateEffect(const std::unordered_set<SharedTrackItem>& items, const std::string& type)
{
    QUndoStack* stack = _MediaBridge.UndoStack();

    QString text("Create '%1' Effect");
    stack->beginMacro(text.arg(type.c_str()));

    for (const SharedTrackItem& item : items)
        stack->push(new CreateTimelineEffectCommand(item, type));

    stack->endMacro();
}

void SequencerController::RemoveTimelineEffects(const std::unordered_set<Effect*>& effects)
{
    std::vector<Effect*> sorted;
    sorted.reserve(effects.size());

    for (const auto& effect : effects)
        sorted.push_back(effect);

    std::sort(sorted.begin(), sorted.end(), [&](const Effect* _a, const Effect* _b) -> bool
    {
        if (_a->GetEffectType() != _b->GetEffectType())
            return _a->GetEffectType() < _b->GetEffectType();

        if (_a->GetEffectType() == Effect::EffectType::ITEM)
        {
            TrackItem* _aitem = _a->TimelineItem();
            TrackItem* _bitem = _b->TimelineItem();

            int _aitemidx = _aitem->Index();
            int _bitemidx = _bitem->Index();

            // Sort ascending based on the track item index -- effect _b belongs to a different track item than _a
            if (_aitemidx != _bitemidx)
                return _aitemidx < _bitemidx;

            // Sort descending based on the effect index
            // we need the items to be reversed such that when deleting them, the index of other items don't change
            // and running undo works as expected
            return _aitem->EffectIndex(_a) > _bitem->EffectIndex(_b);
        }
        else
        {
            PlaybackTrack* _atrk = _a->Track();
            PlaybackTrack* _btrk = _b->Track();

            int _atrkidx = _atrk->Index();
            int _btrkidx = _btrk->Index();

            // Sort ascending based on the track item index -- effect _b belongs to a different track item than _a
            if (_atrkidx != _btrkidx)
                return _atrkidx < _btrkidx;

            // Sort descending based on the effect index
            // we need the items to be reversed such that when deleting them, the index of other items don't change
            // and running undo works as expected
            return _atrk->EffectIndex(_a) > _btrk->EffectIndex(_b);
        }
    });

    QUndoStack* stack = _MediaBridge.UndoStack();
    stack->beginMacro("Delete Timeline Effect(s)");

    for (auto& effect : sorted)
    {
        if (effect->GetEffectType() == Effect::EffectType::TRACK)
            stack->push(new DeleteTrackEffectCommand(effect));
        else
            stack->push(new DeleteTimelineEffectCommand(effect));
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

void SequencerController::ToggleItemState(const std::unordered_set<SharedTrackItem>& items)
{
    QUndoStack* stack = _MediaBridge.UndoStack();
    stack->beginMacro("Toggle TrackItem(s)");

    for (const SharedTrackItem& item : items)
        stack->push(new ToggleTrackItemStateCommand(item));

    stack->endMacro();
}

void SequencerController::ToggleItemState(const std::unordered_set<Effect*>& effects)
{
    QUndoStack* stack = _MediaBridge.UndoStack();
    stack->beginMacro("Toggle Timeline Effect(s)");

    for (Effect* effect : effects)
        if (effect->GetEffectType() == Effect::EffectType::TRACK)
            stack->push(new ToggleTrackEffectCommand(effect));
        else
            stack->push(new ToggleTimelineEffectCommand(effect));

    stack->endMacro();
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

    std::size_t max = track->NumItems();
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
