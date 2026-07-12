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
            item->Track()->RemoveItem(item);

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
    
        // STrackItem* trackitem = current->Item(m_Item);

        // Try move
        if (requested->Track()->AddItem(item, m_Requested))
        {
            VOID_LOG_INFO("Requested has Accepted change...");
            item->Track()->RemoveItem(item);

            requested->AddItem(item);
            current->RemoveItem(item);

            return true;
        }

        return false;
    }

    return false;
}

VOID_NAMESPACE_CLOSE
