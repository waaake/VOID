// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Internal */
#include "SequenceCommands.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/// CreateTrackItemCommand

CreateTrackItemCommand::CreateTrackItemCommand(const SharedMediaClip& media, const SharedPlaybackTrack& track, v_frame_t frame, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Project(media->Project())
    , m_Sequence(track->Sequence())
    , m_TrackIndex(track->Index())
    , m_MediaIndex(m_Project->MediaRow(media))
    , m_Frame(frame)
    , m_TrackType(track->Type())
{
    setText("Add media to track");
}

void CreateTrackItemCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);
        track->RemoveItem(item);
    }
}

bool CreateTrackItemCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        const SharedMediaClip& media = m_Project->MediaAt(m_MediaIndex, 0);
        if (SharedTrackItem item = track->AddMedia(media, m_Frame))
        {
            m_ItemIndex = item->Index();
            return true;
        }
    }

    return false;
}

/// MoveTrackItemCommand

MoveTrackItemCommand::MoveTrackItemCommand(const SharedTrackItem& item, v_frame_t frame, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(item->Track()->Sequence())
    , m_TrackIndex(item->Track()->Index())
    , m_ItemIndex(item->Index())
    , m_Requested(frame)
    , m_Previous(item->TimelineIn())
    , m_TrackType(item->Track()->Type())
{
    setText("Move TrackItem");
}

void MoveTrackItemCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        track->MoveItem(item, m_Previous);
    }
}

bool MoveTrackItemCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        return track->MoveItem(item, m_Requested);
    }

    return false;
}

/// MoveItemToTrackCommand

MoveItemToTrackCommand::MoveItemToTrackCommand(const SharedPlaybackTrack& track, const SharedTrackItem& item, int trackIndex, v_frame_t frame, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(track->Sequence())
    , m_PreviousFrame(item->TimelineIn())
    , m_RequestedFrame(frame)
    , m_PreviousTrackIndex(track->Index())
    , m_RequestedTrackIndex(trackIndex)
    , m_PreviousItemIndex(item->Index())
    , m_Type(track->Type())
{
    setText("Move TrackItem");
}

void MoveItemToTrackCommand::undo()
{
    SharedPlaybackTrack previous = m_Sequence->TrackAt(m_PreviousTrackIndex, m_Type);
    SharedPlaybackTrack requested = m_Sequence->TrackAt(m_RequestedTrackIndex, m_Type);

    if (previous && requested)
    {
        SharedTrackItem item = requested->ItemAt(m_MovedItemIndex);
        // Try move back, should have no issues though
        if (previous->AddItem(item, m_PreviousFrame))
            requested->RemoveItem(item);
    }
}

bool MoveItemToTrackCommand::Redo()
{
    SharedPlaybackTrack previous = m_Sequence->TrackAt(m_PreviousTrackIndex, m_Type);
    SharedPlaybackTrack requested = m_Sequence->TrackAt(m_RequestedTrackIndex, m_Type);

    if (previous && requested)
    {
        SharedTrackItem item = previous->ItemAt(m_PreviousItemIndex);
        if (requested->AddItem(item, m_RequestedFrame))
        {
            m_MovedItemIndex = requested->ItemIndex(item);
            previous->RemoveItem(item);
            return true;
        }
    }
    return false;
}

/// OffsetItemCommand

OffsetItemCommand::OffsetItemCommand(const SharedTrackItem& item, int offset, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(item->Track()->Sequence())
    , m_TrackIndex(item->Track()->Index())
    , m_ItemIndex(item->Index())
    , m_Offset(offset)
    , m_TrackType(item->Track()->Type())
{
    setText("Offset Item");
}

void OffsetItemCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        track->OffsetItem(item, -m_Offset);
    }
}

bool OffsetItemCommand::Redo()
{
    if (m_Offset == 0) return false;

    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        return track->OffsetItem(item, m_Offset);
    }

    return false;
}

/// SetTrackItemColorCommand

SetTrackItemColorCommand::SetTrackItemColorCommand(const SharedTrackItem& item, const QColor& color, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(item->Track()->Sequence())
    , m_TrackIndex(item->Track()->Index())
    , m_ItemIndex(item->Index())
    , m_Color(color)
    , m_Previous(item->Color())
    , m_TrackType(item->Track()->Type())
    , m_Reset(false)
{
    setText("Set Trackitem Color");
}

SetTrackItemColorCommand::SetTrackItemColorCommand(const SharedTrackItem& item, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(item->Track()->Sequence())
    , m_TrackIndex(item->Track()->Index())
    , m_ItemIndex(item->Index())
    , m_Color(QColor(0, 0, 0))
    , m_Previous(item->Color())
    , m_Reset(true)
    , m_TrackType(item->Track()->Type())
{
    setText("Reset Trackitem Color");
}

void SetTrackItemColorCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->SetColor(m_Previous);
    }
}

bool SetTrackItemColorCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        m_Reset ? item->ResetColor() : item->SetColor(m_Color);
        return true;
    }

    return false;
}

/// LockTrackCommand

ToggleLockTrackCommand::ToggleLockTrackCommand(const SharedPlaybackTrack& track, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(track->Sequence())
    , m_TrackIndex(track->Index())
    , m_TrackType(track->Type())
{
    setText(track->Locked() ? "Unlock Track" : "Lock Track");
}

void ToggleLockTrackCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
        track->Lock(!track->Locked());
}

bool ToggleLockTrackCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        track->Lock(!track->Locked());
        return true;
    }

    return false;
}

/// ToggleTrackStateCommand

ToggleTrackStateCommand::ToggleTrackStateCommand(const SharedPlaybackTrack& track, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(track->Sequence())
    , m_TrackIndex(track->Index())
    , m_TrackType(track->Type())
{
    setText(track->Enabled() ? "Disable Track" : "Enable Track");
}

void ToggleTrackStateCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
        track->SetEnabled(!track->Enabled());
}

bool ToggleTrackStateCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        track->SetEnabled(!track->Enabled());
        return true;
    }

    return false;
}

/// ToggleTrackItemStateCommand

ToggleTrackItemStateCommand::ToggleTrackItemStateCommand(const SharedTrackItem& item, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(item->Track()->Sequence())
    , m_ItemIndex(item->Track()->ItemIndex(item))
    , m_TrackType(item->Track()->Type())
    , m_TrackIndex(item->Track()->Index())
{
    setText("Toggle TrackItem");
}

void ToggleTrackItemStateCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->SetEnabled(!item->Enabled());
    }
}

bool ToggleTrackItemStateCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->SetEnabled(!item->Enabled());
        return true;
    }
    return false;
}

/// ToggleTimelineEffectCommand

ToggleTimelineEffectCommand::ToggleTimelineEffectCommand(Effect* effect, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(effect->TimelineItem()->Track()->Sequence())
{
    const PlaybackTrack* const track = effect->TimelineItem()->Track();
    const TrackItem* const item = effect->TimelineItem();
    m_TrackType = track->Type();
    m_TrackIndex = track->Index();
    m_ItemIndex = item->Index();
    m_EffectIndex = item->EffectIndex(effect);
}

void ToggleTimelineEffectCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);
        Effect* effect = item->EffectAt(m_EffectIndex);
        effect->SetEnabled(!effect->Enabled());
    }
}

bool ToggleTimelineEffectCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);
        Effect* effect = item->EffectAt(m_EffectIndex);
        effect->SetEnabled(!effect->Enabled());
        return true;
    }
    return false;
}

/// ToggleTrackEffectCommand

ToggleTrackEffectCommand::ToggleTrackEffectCommand(Effect* effect, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(effect->Track()->Sequence())
{
    const PlaybackTrack* const track = effect->Track();
    m_TrackType = track->Type();
    m_TrackIndex = track->Index();
    m_EffectIndex = track->EffectIndex(effect);
}

void ToggleTrackEffectCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        Effect* effect = track->EffectAt(m_EffectIndex);
        effect->SetEnabled(!effect->Enabled());
    }
}

bool ToggleTrackEffectCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        Effect* effect = track->EffectAt(m_EffectIndex);
        effect->SetEnabled(!effect->Enabled());
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

DeleteTrackCommand::DeleteTrackCommand(const SharedPlaybackTrack& track, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(track->Sequence())
    , m_Type(track->Type())
{
    m_TrackIndex = track->Index();
    setText("Delete Track");
}

void DeleteTrackCommand::undo()
{
    std::istringstream is(m_TrackData, std::ios::binary);
    SharedPlaybackTrack track = std::make_shared<PlaybackTrack>(m_Type, m_Sequence);
    track->Deserialize(is);
    m_Type == Sequence::TrackType::VIDEO ? m_Sequence->AddVideoTrack(track, m_TrackIndex) : m_Sequence->AddAudioTrack(track, m_TrackIndex);
}

bool DeleteTrackCommand::Redo()
{
    if (SharedPlaybackTrack track = m_Sequence->TrackAt(m_TrackIndex, m_Type))
    {
        std::ostringstream os(std::ios::binary);
        track->Serialize(os);
        m_TrackData = os.str();

        track->Clear();
        m_Sequence->RemoveTrack(track);
        return true;
    }

    return false;
}

/// DeleteTrackItemCommand

DeleteTrackItemCommand::DeleteTrackItemCommand(const SharedTrackItem& item, QUndoCommand* parent)
    : VoidUndoCommand(parent)
{
    PlaybackTrack* track = item->Track();
    m_Sequence = track->Sequence();
    m_TrackType = track->Type();
    m_TrackIndex = track->Index();
    m_ItemIndex = item->Index();

    setText("Delete TrackItem");
}

void DeleteTrackItemCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        std::istringstream is(m_ItemData, std::ios::binary);
        SharedTrackItem item = std::make_shared<TrackItem>(track.get());
        item->Deserialize(is);
        track->AddItem(item);
    }
}

bool DeleteTrackItemCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        std::ostringstream os(std::ios::binary);
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->Serialize(os);
        m_ItemData = os.str();
        track->RemoveItem(item);
        return true;
    }
    return false;
}

/// DeleteTimelineEffectCommand

DeleteTimelineEffectCommand::DeleteTimelineEffectCommand(Effect* effect, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(effect->TimelineItem()->Track()->Sequence())
{
    const PlaybackTrack* const track = effect->TimelineItem()->Track();
    const TrackItem* const item = effect->TimelineItem();
    m_TrackType = track->Type();
    m_TrackIndex = m_TrackType == Sequence::TrackType::VIDEO ? m_Sequence->VideoTrackIndex(track) : m_Sequence->AudioTrackIndex(track);
    m_ItemIndex = track->ItemIndex(item);
    m_EffectIndex = item->EffectIndex(effect);

    m_EffectType = effect->Type();

    setText("Delete Timeline Effect");
}

void DeleteTimelineEffectCommand::undo()
{
    const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType);
    if (track)
    {
        std::istringstream is(m_EffectData, std::ios::binary);
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);
        Effect* effect = EffectsBridge::Instance().CreateEffect(m_EffectType);
        effect->Deserialize(is);
        item->InsertEffect(effect, m_EffectIndex);
    }
}

bool DeleteTimelineEffectCommand::Redo()
{
    const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType);
    if (track)
    {
        std::ostringstream os(std::ios::binary);
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);
        Effect* effect = item->EffectAt(m_EffectIndex);
        effect->Serialize(os);
        m_EffectData = os.str();
        item->RemoveEffect(m_EffectIndex);
        return true;
    }
    return false;
}

/// DeleteTrackEffectCommand

DeleteTrackEffectCommand::DeleteTrackEffectCommand(Effect* effect, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(effect->Track()->Sequence())
{
    const PlaybackTrack* const track = effect->Track();
    m_TrackType = track->Type();
    m_TrackIndex = m_TrackType == Sequence::TrackType::VIDEO ? m_Sequence->VideoTrackIndex(track) : m_Sequence->AudioTrackIndex(track);
    m_EffectIndex = track->EffectIndex(effect);
    m_EffectType = effect->Type();

    setText("Delete Timeline Effect");
}

void DeleteTrackEffectCommand::undo()
{
    const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType);
    if (track)
    {
        std::istringstream is(m_EffectData, std::ios::binary);
        Effect* effect = EffectsBridge::Instance().CreateEffect(m_EffectType);
        effect->Deserialize(is);
        track->InsertEffect(effect, m_EffectIndex);
    }
}

bool DeleteTrackEffectCommand::Redo()
{
    const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType);
    if (track)
    {
        std::ostringstream os(std::ios::binary);
        Effect* effect = track->EffectAt(m_EffectIndex);
        effect->Serialize(os);
        m_EffectData = os.str();
        track->RemoveEffect(m_EffectIndex, true);
        return true;
    }
    return false;
}

/// CreateTimelineEffectCommand

CreateTimelineEffectCommand::CreateTimelineEffectCommand(const SharedTrackItem& item, const std::string type, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(item->Track()->Sequence())
    , m_EffectType(type)
{
    const PlaybackTrack* const track = item->Track();
    m_TrackType = track->Type();
    m_TrackIndex = track->Index();
    m_ItemIndex = track->ItemIndex(item);
    m_EffectIndex = item->NumEffects();

    QString text("Create '%1' Effect");
    setText(text.arg(type.c_str()));
}

void CreateTimelineEffectCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->RemoveEffect(m_EffectIndex);
    }
}

bool CreateTimelineEffectCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        const SharedTrackItem item = track->ItemAt(m_ItemIndex);

        if (m_Name.empty())
        {
            if (Effect* effect = track->CreateEffect(item, m_EffectType))
            {
                m_Name = effect->Name();
                return true;
            }
        }
        else
        {
            return (bool)track->CreateEffect(item, m_EffectType, m_Name);
        }
    }
    return false;
}

/// CreateTrackEffectCommand

CreateTrackEffectCommand::CreateTrackEffectCommand(const SharedPlaybackTrack& track, const std::string type, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(track->Sequence())
    , m_EffectType(type)
{
    m_TrackType = track->Type();
    m_TrackIndex = track->Index();
    m_EffectIndex = track->NumEffects();

    QString text("Create '%1' Effect");
    setText(text.arg(type.c_str()));
}

void CreateTrackEffectCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
        track->RemoveEffect(m_EffectIndex);
}

bool CreateTrackEffectCommand::Redo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        if (m_Name.empty())
        {
            if (Effect* effect = track->CreateEffect(m_EffectType))
            {
                m_Name = effect->Name();
                return true;
            }
        }
        else
        {
            return (bool)track->CreateEffect(m_EffectType, m_Name);
        }
    }
    return false;
}

/// RazorTrackCommand

RazorTrackCommand::RazorTrackCommand(const SharedPlaybackTrack& track, v_frame_t frame, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(track->Sequence())
    , m_TrackIndex(track->Index())
    , m_Type(track->Type())
    , m_Frame(frame)
{
    setText("Razor Item");
}

void RazorTrackCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_Type))
        track->MergeCut(m_Frame);
}

bool RazorTrackCommand::Redo()
{
    const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_Type);
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
    , m_TrackIndex(track->Index())
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
    , m_TrackIndex(item->Track()->Index())
    , m_ItemIndex(item->Track()->ItemIndex(item))
    , m_Offset(offset)
    , m_Previous(item->SourceIn())
{
    setText("Slip Item Source");
}

void OffsetItemSourceCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->SetSourceIn(m_Previous);
    }
}

bool OffsetItemSourceCommand::Redo()
{
    if (m_Offset == 0)
        return false;

    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->SetSourceIn(item->SourceIn() + m_Offset);
        return true;
    }

    return false;
}

/// TrimItemHeadCommand

TrimItemHeadCommand::TrimItemHeadCommand(const SharedTrackItem& item, int handle, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(item->Track()->Sequence())
    , m_TrackType(item->Track()->Type())
    , m_TrackIndex(item->Track()->Index())
    , m_ItemIndex(item->Track()->ItemIndex(item))
    , m_Handle(handle)
{
    setText("Trim Item");
}

void TrimItemHeadCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->TrimHead(-m_Handle);
    }
}

bool TrimItemHeadCommand::Redo()
{
    if (m_Handle == 0) return false;

    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        if (m_Handle > item->Duration()) return false;
        item->TrimHead(m_Handle);
        return true;
    }

    return false;
}

/// TrimItemTailCommand

TrimItemTailCommand::TrimItemTailCommand(const SharedTrackItem& item, int handle, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Sequence(item->Track()->Sequence())
    , m_TrackType(item->Track()->Type())
    , m_TrackIndex(item->Track()->Index())
    , m_ItemIndex(item->Track()->ItemIndex(item))
    , m_Handle(handle)
{
    setText("Trim Item");
}

void TrimItemTailCommand::undo()
{
    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        item->TrimTail(-m_Handle);
    }
}

bool TrimItemTailCommand::Redo()
{
    if (m_Handle == 0) return false;

    if (const SharedPlaybackTrack& track = m_Sequence->TrackAt(m_TrackIndex, m_TrackType))
    {
        SharedTrackItem item = track->ItemAt(m_ItemIndex);
        if (m_Handle > item->Duration()) return false;
        item->TrimTail(m_Handle);
        return true;
    }

    return false;
}

/// CutPasteTrackItemCommand

CutPasteTrackItemCommand::CutPasteTrackItemCommand(const Sequence::Context& source, const Sequence::Context& destination, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_SourceCtx(source)
    , m_DestinationCtx(destination)
{
    setText("Cut TrackItem");
}

void CutPasteTrackItemCommand::undo()
{
    // The source track item has been moved, can't resolve back to the track item object
    Sequence::ResolvedContext rsource = m_SourceCtx.Resolve(Sequence::Context::Type::TRACK);
    Sequence::ResolvedContext rdest = m_ActedCtx.Resolve();
    if (rdest.track.get() == rsource.track.get())
    {
        rsource.track->MoveItem(rdest.trackItem, rsource.frame);
        return;
    }

    if (rsource.track->AddItem(rdest.trackItem, rsource.frame))
    {
        rdest.track->RemoveItem(rdest.trackItem);
    }
}

bool CutPasteTrackItemCommand::Redo()
{
    // Ensure that we have a Track Context to copy the item to
    if (m_DestinationCtx.type == Sequence::Context::Type::TRACK)
    {
        Sequence::ResolvedContext rsource = m_SourceCtx.Resolve();
        Sequence::ResolvedContext rdest = m_DestinationCtx.Resolve();

        if (rdest.track.get() == rsource.track.get())
        {
            rdest.track->MoveItem(rsource.trackItem, rdest.frame);
            m_ActedCtx = Sequence::Context::Get(rsource.trackItem);
            return true;
        }

        if (rdest.track->AddItem(rsource.trackItem, rdest.frame))
        {
            rsource.track->RemoveItem(rsource.trackItem);
            m_ActedCtx = Sequence::Context::Get(rsource.trackItem);
            return true;
        }
    }
    return false;
}

/// CopyPasteTrackItemCommand

CopyPasteTrackItemCommand::CopyPasteTrackItemCommand(const Sequence::Context& source, const Sequence::Context& destination, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_SourceCtx(source)
    , m_DestinationCtx(destination)
{
    setText("Copy TrackItem");
}

void CopyPasteTrackItemCommand::undo()
{
    Sequence::ResolvedContext acted = m_ActedCtx.Resolve();
    acted.track->RemoveItem(acted.trackItem);
}

bool CopyPasteTrackItemCommand::Redo()
{
    // Ensure that we have a Track Context to copy the item to
    if (m_DestinationCtx.type == Sequence::Context::Type::TRACK)
    {
        Sequence::ResolvedContext rsource = m_SourceCtx.Resolve();
        Sequence::ResolvedContext rdest = m_DestinationCtx.Resolve();

        SharedTrackItem copied = std::make_shared<TrackItem>(*rsource.trackItem.get());
        if (rdest.track->AddItem(copied, rdest.frame))
        {
            // Store the acted upon context
            m_ActedCtx = Sequence::Context::Get(copied);
            return true;
        }
    }
    return false;
}

VOID_NAMESPACE_CLOSE
