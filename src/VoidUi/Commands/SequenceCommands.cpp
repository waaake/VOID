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
            current->Track()->RemoveItem(m_Requested);

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
            current->Track()->RemoveItem(m_Previous);

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

VOID_NAMESPACE_CLOSE
