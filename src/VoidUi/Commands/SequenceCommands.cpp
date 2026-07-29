// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SequenceCommands.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidUi/Sequencer/Graphics/STrack.h"
#include "VoidUi/Sequencer/Graphics/STrackItem.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

MoveTrackItemCommand::MoveTrackItemCommand(SharedTrackItem& item, v_frame_t frame, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Item(item)
    , m_Requested(frame)
    , m_Previous(item->TimelineIn())
{
    setText("Move TrackItem");
}

void MoveTrackItemCommand::undo()
{
    if (SharedTrackItem item = m_Item.lock())
    {
        if (PlaybackTrack* track = item->Track())
            track->MoveItem(item, m_Previous);
    }
}

bool MoveTrackItemCommand::Redo()
{
    if (SharedTrackItem item = m_Item.lock())
    {
        if (PlaybackTrack* track = item->Track())
            return track->MoveItem(item, m_Requested);
        
        return false;
    }
    return false;
}

/// Move Item to Track

MoveItemToTrackCommand::MoveItemToTrackCommand(
    SequencerController* controller,
    SharedTrackItem& item,
    int currentTrackIndex,
    int trackIndex,
    v_frame_t frame,
    QUndoCommand* parent
)
    : VoidUndoCommand(parent)
    , m_Controller(controller)
    , m_Item(item)
    , m_CurrentTrackIndex(currentTrackIndex)
    , m_TrackIndex(trackIndex)
    , m_Requested(frame)
    , m_Previous(item->TimelineIn())
{
    setText("Move TrackItem");
}

void MoveItemToTrackCommand::undo()
{
    if (SharedTrackItem item = m_Item.lock())
    {
        STrack* previous = m_Controller->TrackAt(m_CurrentTrackIndex);
        STrack* current = m_Controller->TrackAt(m_TrackIndex);

        // Try move back, should have no issues though
        if (previous->Track()->AddItem(item, m_Previous))
        {
            current->Track()->RemoveItem(item);

            // Visual changes
            previous->AddItem(item);
            current->RemoveItem(item);
        }
    }
}

bool MoveItemToTrackCommand::Redo()
{
    if (SharedTrackItem item = m_Item.lock())
    {
        STrack* current = m_Controller->TrackAt(m_CurrentTrackIndex);
        STrack* requested = m_Controller->TrackAt(m_TrackIndex);

        // Try move
        if (requested->Track()->AddItem(item, m_Requested))
        {
            current->Track()->RemoveItem(item);

            // Visual updates
            requested->AddItem(item);
            current->RemoveItem(item);

            return true;
        }

        return false;
    }

    return false;
}

/// SetTrackItemColorCommand

SetTrackItemColorCommand::SetTrackItemColorCommand(const SharedTrackItem& item, const QColor& color, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Item(item)
    , m_Color(color)
    , m_Previous(item->Color())
    , m_Reset(false)
{
    setText("Set Trackitem Color");
}

SetTrackItemColorCommand::SetTrackItemColorCommand(const SharedTrackItem& item, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Item(item)
    , m_Color(QColor(0, 0, 0))
    , m_Previous(item->Color())
    , m_Reset(true)
{
    setText("Reset Trackitem Color");
}

void SetTrackItemColorCommand::undo()
{
    if (SharedTrackItem item = m_Item.lock()) item->SetColor(m_Previous);
}

bool SetTrackItemColorCommand::Redo()
{
    if (SharedTrackItem item = m_Item.lock())
    {
        m_Reset ? item->ResetColor() : item->SetColor(m_Color);
        return true;
    }

    return false;
}

/// LockTrackCommand

ToggleLockTrackCommand::ToggleLockTrackCommand(const SharedPlaybackTrack& track, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Track(track)
    , m_Previous(track->Locked())
{
    setText(m_Previous ? "Unlock Track" : "Lock Track");
}

void ToggleLockTrackCommand::undo()
{
    if (SharedPlaybackTrack track = m_Track.lock()) track->Lock(m_Previous);
}

bool ToggleLockTrackCommand::Redo()
{
    if (SharedPlaybackTrack track = m_Track.lock())
    {
        track->Lock(!m_Previous);
        return true;
    }

    return false;
}

/// ToggleTrackStateCommand

ToggleTrackStateCommand::ToggleTrackStateCommand(const SharedPlaybackTrack& track, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Track(track)
    , m_Previous(track->Enabled())
{
    setText(m_Previous ? "Disable Track" : "Enable Track");
}

void ToggleTrackStateCommand::undo()
{
    if (SharedPlaybackTrack track = m_Track.lock()) track->SetEnabled(m_Previous);
}

bool ToggleTrackStateCommand::Redo()
{
    if (SharedPlaybackTrack track = m_Track.lock())
    {
        track->SetEnabled(!m_Previous);
        return true;
    }

    return false;
}

/// CreateTrackCommand

CreateTrackCommand::CreateTrackCommand(const SharedPlaybackSequence& sequence, const Sequence::TrackType& type, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(sequence)
    , m_Type(type)
    , m_Index(type == Sequence::TrackType::VIDEO ? sequence->NumVideoTracks() : sequence->NumAudioTracks())
{
    setText(type == Sequence::TrackType::VIDEO ? "Create Video Track" : "Create Audio Track");
}

void CreateTrackCommand::undo()
{
    if (SharedPlaybackSequence sequence = m_Sequence.lock()) sequence->RemoveTrack(m_Index, m_Type);
}

bool CreateTrackCommand::Redo()
{
    if (SharedPlaybackSequence sequence = m_Sequence.lock())
    {
        sequence->CreateTrack(m_Type);
        return true;
    }
    return false;
}

/// DeleteTrackCommand

DeleteTrackCommand::DeleteTrackCommand(const SharedPlaybackSequence& sequence, int index, const Sequence::TrackType& type, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(sequence)
    , m_Type(type)
    , m_TrackIndex(index)
{
    setText("Delete Track");
}

void DeleteTrackCommand::undo()
{
    if (SharedPlaybackSequence sequence = m_Sequence.lock())
    {
        SharedPlaybackTrack track = sequence->CreateTrack(m_Type, m_TrackIndex);
        track->Deserialize(m_TrackData);
    }
}

bool DeleteTrackCommand::Redo()
{
    if (SharedPlaybackSequence sequence = m_Sequence.lock())
    {
        rapidjson::Document doc;
        doc.SetObject();

        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator = doc.GetAllocator();
        if (const SharedPlaybackTrack track = sequence->VideoTrackAt(m_TrackIndex))
        {
            track->Serialize(m_TrackData, allocator);
            track->Clear();
            sequence->RemoveTrack(track);
            return true;
        }

        return false;
    }

    return false;
}

/// DeleteTrackItemCommand

DeleteTrackItemCommand::DeleteTrackItemCommand(const SharedPlaybackSequence& sequence, const Sequence::TrackType& type, int trackindex, int index, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Type(type)
    , m_TrackIndex(trackindex)
    , m_ItemIndex(index)
{
    setText("Delete TrackItem");
}

void DeleteTrackItemCommand::undo()
{
    if (SharedPlaybackSequence sequence = m_Sequence.lock())
    {
        if (const SharedPlaybackTrack track = sequence->VideoTrackAt(m_TrackIndex))
        {
            SharedTrackItem item = std::make_shared<TrackItem>(track.get());
            track->AddItem(item);
        }
    }
}

bool DeleteTrackItemCommand::Redo()
{
    if (SharedPlaybackSequence sequence = m_Sequence.lock())
    {
        rapidjson::Document doc;
        doc.SetObject();

        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator = doc.GetAllocator();
        if (const SharedPlaybackTrack track = sequence->VideoTrackAt(m_TrackIndex))
        {
            const SharedTrackItem item = track->ItemAt(m_ItemIndex);
            item->Serialize(m_ItemData, allocator);
            track->RemoveItem(item);
            return true;
        }
        return false;
    }
    return false;
}

/// RazorTrackCommand

RazorTrackCommand::RazorTrackCommand(const SharedPlaybackTrack& track, v_frame_t frame, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(track->Sequence())
    , m_TrackIndex(track->TrackIndex())
    , m_Type(track->Type())
    , m_Frame(frame)
{
    setText("Razor Item");
}

void RazorTrackCommand::undo()
{
    SharedPlaybackTrack track = m_Type == Sequence::TrackType::VIDEO ? m_Sequence->VideoTrackAt(m_TrackIndex) : m_Sequence->AudioTrackAt(m_TrackIndex);
    track->MergeCut(m_Frame);
}

bool RazorTrackCommand::Redo()
{
    SharedPlaybackTrack track = m_Type == Sequence::TrackType::VIDEO ? m_Sequence->VideoTrackAt(m_TrackIndex) : m_Sequence->AudioTrackAt(m_TrackIndex);
    return track ? track->RazorAt(m_Frame) : false;
}

/// RazorSequenceCommand

RazorSequenceCommand::RazorSequenceCommand(const SharedPlaybackSequence& sequence, v_frame_t frame, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(sequence)
    , m_Frame(frame)
{
    setText("Razor Items");
}

void RazorSequenceCommand::undo()
{
    for (auto& track : m_Sequence->VideoTracks())
        track->MergeCut(m_Frame);

    for (auto& track : m_Sequence->AudioTracks())
        track->MergeCut(m_Frame);
}

bool RazorSequenceCommand::Redo()
{
    bool status = false;

    for (auto& track : m_Sequence->VideoTracks())
        status |= track->RazorAt(m_Frame);

    for (auto& track : m_Sequence->AudioTracks())
        status |= track->RazorAt(m_Frame);

    return status;
}

/// MergeCutCommand

MergeCutCommand::MergeCutCommand(const SharedPlaybackTrack& track, v_frame_t frame, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(track->Sequence())
    , m_TrackIndex(track->TrackIndex())
    , m_Type(track->Type())
    , m_Frame(frame)
{
    setText("Merge Items");
}

void MergeCutCommand::undo()
{
    SharedPlaybackTrack track = m_Type == Sequence::TrackType::VIDEO ? m_Sequence->VideoTrackAt(m_TrackIndex) : m_Sequence->AudioTrackAt(m_TrackIndex);
    track->RazorAt(m_Frame);
}

bool MergeCutCommand::Redo()
{
    SharedPlaybackTrack track = m_Type == Sequence::TrackType::VIDEO ? m_Sequence->VideoTrackAt(m_TrackIndex) : m_Sequence->AudioTrackAt(m_TrackIndex);
    return track ? track->MergeCut(m_Frame) : false;
}

/// OffsetItemSourceCommand

OffsetItemSourceCommand::OffsetItemSourceCommand(const SharedTrackItem& item, int offset, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(item->Track()->Sequence())
    , m_TrackType(item->Track()->Type())
    , m_TrackIndex(item->Track()->TrackIndex())
    , m_ItemIndex(item->Track()->ItemIndex(item))
    , m_Offset(offset)
    , m_Previous(item->SourceIn())
{
    setText("Slip Item Source");
}

void OffsetItemSourceCommand::undo()
{
    SharedPlaybackTrack track = m_TrackType == Sequence::TrackType::VIDEO ? m_Sequence->VideoTrackAt(m_TrackIndex) : m_Sequence->AudioTrackAt(m_TrackIndex);
    if (track)
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->SetSourceIn(m_Previous);
    }
}

bool OffsetItemSourceCommand::Redo()
{
    if (m_Offset == 0)
        return false;

    SharedPlaybackTrack track = m_TrackType == Sequence::TrackType::VIDEO ? m_Sequence->VideoTrackAt(m_TrackIndex) : m_Sequence->AudioTrackAt(m_TrackIndex);
    if (track)
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->SetSourceIn(item->SourceIn() + m_Offset);
        return true;
    }

    return false;
}

VOID_NAMESPACE_CLOSE
