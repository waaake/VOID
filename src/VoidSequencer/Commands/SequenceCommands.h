// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCE_COMMANDS_H
#define _SEQUENCE_COMMANDS_H

/* Qt */
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Effects/Bridge.h"
#include "VoidObjects/Media/Tag.h"
#include "VoidObjects/Project/Project.h"
#include "VoidObjects/Sequence/Context.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/TrackItem.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidUndo/VoidCommand.h"

VOID_NAMESPACE_OPEN

class CreateTrackItemCommand : public VoidUndoCommand
{
public:
    CreateTrackItemCommand(const SharedMediaClip& media, const SharedPlaybackTrack& track, v_frame_t frame, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Core::Project* m_Project;
    PlaybackSequence* m_Sequence;
    v_frame_t m_Frame;
    int m_TrackIndex, m_MediaIndex, m_ItemIndex;
    Sequence::Type m_TrackType;
};

class CreateTrackItemsCommand : public VoidUndoCommand
{
public:
    CreateTrackItemsCommand(const std::vector<SharedMediaClip>& clips, const SharedPlaybackTrack& track, v_frame_t frame, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_TrackContext;
    std::vector<int> m_ItemIndexes;
    std::vector<int> m_MediaIndexes;
    v_frame_t m_Frame;
};

class MoveTrackItemCommand : public VoidUndoCommand
{
public:
    MoveTrackItemCommand(const SharedTrackItem& item, v_frame_t frame, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    int m_TrackIndex, m_ItemIndex;
    v_frame_t m_Requested, m_Previous;
    Sequence::Type m_TrackType;
};

class MoveItemToTrackCommand : public VoidUndoCommand
{
public:
    MoveItemToTrackCommand(const SharedPlaybackTrack& track, const SharedTrackItem& item, int trackIndex, v_frame_t frame, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    v_frame_t m_PreviousFrame, m_RequestedFrame;
    int m_PreviousTrackIndex, m_RequestedTrackIndex;
    int m_PreviousItemIndex, m_MovedItemIndex;
    Sequence::Type m_Type;
};

class OffsetItemCommand : public VoidUndoCommand
{
public:
    OffsetItemCommand(const SharedTrackItem& item, int offset, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    int m_TrackIndex, m_ItemIndex;
    int m_Offset;
    Sequence::Type m_TrackType;
};

class SetTrackItemColorCommand : public VoidUndoCommand
{
public:
    SetTrackItemColorCommand(const SharedTrackItem& item, const QColor& color, QUndoCommand* parent = nullptr);
    SetTrackItemColorCommand(const SharedTrackItem& item, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    QColor m_Color;
    QColor m_Previous;
    PlaybackSequence* m_Sequence;
    int m_TrackIndex, m_ItemIndex;
    Sequence::Type m_TrackType;
    bool m_Reset;
};

class ToggleLockTrackCommand : public VoidUndoCommand
{
public:
    explicit ToggleLockTrackCommand(const SharedPlaybackTrack& track, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    Sequence::Type m_TrackType;
};

class ToggleTrackStateCommand : public VoidUndoCommand
{
public:
    explicit ToggleTrackStateCommand(const SharedPlaybackTrack& track, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    Sequence::Type m_TrackType;
};

class ToggleTrackItemStateCommand : public VoidUndoCommand
{
public:
    explicit ToggleTrackItemStateCommand(const SharedTrackItem& item, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    int m_ItemIndex;
    Sequence::Type m_TrackType;
};

class ToggleTimelineEffectCommand : public VoidUndoCommand
{
public:
    explicit ToggleTimelineEffectCommand(Effect* effect, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    int m_ItemIndex;
    int m_EffectIndex;
    Sequence::Type m_TrackType;
};

class ToggleTrackEffectCommand : public VoidUndoCommand
{
public:
    explicit ToggleTrackEffectCommand(Effect* effect, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    int m_EffectIndex;
    Sequence::Type m_TrackType;
};

class CreateTrackCommand : public VoidUndoCommand
{
public:
    CreateTrackCommand(const SharedPlaybackSequence& sequence, const Sequence::Type& type, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::weak_ptr<PlaybackSequence> m_Sequence;
    Sequence::Type m_Type;
    int m_Index;
};

class DeleteTrackCommand : public VoidUndoCommand
{
public:
    DeleteTrackCommand(const SharedPlaybackTrack& track, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::string m_TrackData;
    PlaybackSequence* m_Sequence;
    Sequence::Type m_Type;
    int m_TrackIndex;
};

class DeleteTrackItemCommand : public VoidUndoCommand
{
public:
    DeleteTrackItemCommand(const SharedTrackItem& item, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::string m_ItemData;
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    int m_ItemIndex;
    Sequence::Type m_TrackType;
};

class DeleteTimelineEffectCommand : public VoidUndoCommand
{
public:
    explicit DeleteTimelineEffectCommand(Effect* effect, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::string m_EffectData, m_EffectType;
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    int m_ItemIndex;
    int m_EffectIndex;
    Sequence::Type m_TrackType;
};

class DeleteTrackEffectCommand : public VoidUndoCommand
{
public:
    explicit DeleteTrackEffectCommand(Effect* effect, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::string m_EffectData, m_EffectType;
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    int m_EffectIndex;
    Sequence::Type m_TrackType;
};

class CreateTimelineEffectCommand : public VoidUndoCommand
{
public:
    CreateTimelineEffectCommand(const SharedTrackItem& item, const std::string type, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::string m_EffectType;
    std::string m_Name;
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    int m_ItemIndex;
    int m_EffectIndex;
    Sequence::Type m_TrackType;
};

class CreateTrackEffectCommand : public VoidUndoCommand
{
public:
    CreateTrackEffectCommand(const SharedPlaybackTrack& track, const std::string type, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::string m_EffectType;
    std::string m_Name;
    PlaybackSequence* m_Sequence;
    int m_TrackIndex;
    int m_EffectIndex;
    Sequence::Type m_TrackType;
};

class RazorTrackCommand : public VoidUndoCommand
{
public:
    RazorTrackCommand(const SharedPlaybackTrack& track, v_frame_t frame, QUndoCommand* parent = nullptr);
    RazorTrackCommand(const PlaybackTrack* track, v_frame_t frame, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    Sequence::Type m_Type;
    int m_TrackIndex;
    v_frame_t m_Frame;
};

class RazorSequenceCommand : public VoidUndoCommand
{
public:
    RazorSequenceCommand(const SharedPlaybackSequence& sequence, v_frame_t frame, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    SharedPlaybackSequence m_Sequence;
    v_frame_t m_Frame;
};

class MergeCutCommand : public VoidUndoCommand
{
public:
    MergeCutCommand(const SharedPlaybackTrack& track, v_frame_t frame, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    Sequence::Type m_Type;
    int m_TrackIndex;
    v_frame_t m_Frame;
};

class OffsetItemSourceCommand : public VoidUndoCommand
{
public:
    OffsetItemSourceCommand(const SharedTrackItem& item, int offset, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    Sequence::Type m_TrackType;
    int m_TrackIndex;
    int m_ItemIndex;
    int m_Offset;
    int m_Previous;
};

class TrimItemHeadCommand : public VoidUndoCommand
{
public:
    TrimItemHeadCommand(const SharedTrackItem& item, int handle, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    Sequence::Type m_TrackType;
    int m_TrackIndex;
    int m_ItemIndex;
    int m_Handle;
};

class TrimItemTailCommand : public VoidUndoCommand
{
public:
    TrimItemTailCommand(const SharedTrackItem& item, int handle, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    Sequence::Type m_TrackType;
    int m_TrackIndex;
    int m_ItemIndex;
    int m_Handle;
};

class CutPasteTrackItemCommand : public VoidUndoCommand
{
public:
    CutPasteTrackItemCommand(const Sequence::Context& source, const Sequence::Context& destination, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_SourceCtx;
    Sequence::Context m_DestinationCtx;
    Sequence::Context m_ActedCtx;
};

class CopyPasteTrackItemCommand : public VoidUndoCommand
{
public:
    CopyPasteTrackItemCommand(const Sequence::Context& source, const Sequence::Context& destination, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_SourceCtx;
    Sequence::Context m_DestinationCtx;
    Sequence::Context m_ActedCtx;
};

class CutPasteTrackCommand : public VoidUndoCommand
{
public:
    CutPasteTrackCommand(const Sequence::Context& source, const Sequence::Context& destination, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_SourceCtx;
    Sequence::Context m_DestinationCtx;
    Sequence::Context m_ActedCtx;
};

class CopyPasteTrackCommand : public VoidUndoCommand
{
public:
    CopyPasteTrackCommand(const Sequence::Context& source, const Sequence::Context& destination, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_SourceCtx;
    Sequence::Context m_DestinationCtx;
    Sequence::Context m_ActedCtx;
};

class VersionUpTrackItemCommand : public VoidUndoCommand
{
public:
    explicit VersionUpTrackItemCommand(const SharedTrackItem& item, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_ItemContext;
};

class MaxVersionTrackItemCommand : public VoidUndoCommand
{
public:
    explicit MaxVersionTrackItemCommand(const SharedTrackItem& item, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_ItemContext;
    int m_PreviousRow;
};

class VersionDownTrackItemCommand : public VoidUndoCommand
{
public:
    explicit VersionDownTrackItemCommand(const SharedTrackItem& item, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_ItemContext;
};

class MinVersionTrackItemCommand : public VoidUndoCommand
{
public:
    explicit MinVersionTrackItemCommand(const SharedTrackItem& item, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_ItemContext;
    int m_PreviousRow;
};

class ResetTrackItemMediaCommand : public VoidUndoCommand
{
public:
    ResetTrackItemMediaCommand(const SharedTrackItem& item, const SharedMediaClip& media, QUndoCommand* parent = nullptr);
    ResetTrackItemMediaCommand(const SharedTrackItem& item, const QModelIndex& index, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_ItemContext;
    int m_RequestedRow;
    int m_PreviousRow;
};

class AddTagCommand : public VoidUndoCommand
{
public:
    AddTagCommand(const SharedPlaybackTrack& track, const std::string& tag, const TagMetaStruct& metadata, QUndoCommand* parent = nullptr);
    AddTagCommand(PlaybackTrack* track, const std::string& tag, const TagMetaStruct& metadata, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_TrackContext;
    std::string m_Tag;
    TagMetaStruct m_Metadata;
    int m_TagIndex;
};

class AddTrackItemTagCommand : public VoidUndoCommand
{
public:
    AddTrackItemTagCommand(const SharedTrackItem& trackitem, const std::string& tag, const TagMetaStruct& metadata, QUndoCommand* parent = nullptr);
    AddTrackItemTagCommand(TrackItem* trackitem, const std::string& tag, const TagMetaStruct& metadata, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_ItemContext;
    std::string m_Tag;
    TagMetaStruct m_Metadata;
    int m_TagIndex;
};

class RemoveTrackTagCommand : public VoidUndoCommand
{
public:
    RemoveTrackTagCommand(const SharedPlaybackTrack& track, const QModelIndex& index, QUndoCommand* parent = nullptr);
    RemoveTrackTagCommand(PlaybackTrack* track, const QModelIndex& index, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::string m_TagData;
    Sequence::Context m_TrackContext;
    QModelIndex m_TagIndex;
};

class RemoveTrackItemTagCommand : public VoidUndoCommand
{
public:
    RemoveTrackItemTagCommand(const SharedTrackItem& item, const QModelIndex& index, QUndoCommand* parent = nullptr);
    RemoveTrackItemTagCommand(TrackItem* item, const QModelIndex& index, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::string m_TagData;
    Sequence::Context m_ItemContext;
    QModelIndex m_TagIndex;
};

class SetReferenceMediaCommand : public VoidUndoCommand
{
public:
    SetReferenceMediaCommand(const SharedMediaClip& media, const SharedPlaybackSequence& sequence, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_Context;
    Core::Project* m_Project;
    int m_MediaIndex;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCE_COMMANDS_H
