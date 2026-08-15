// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/TrackItem.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidObjects/Effects/Bridge.h"
#include "VoidUndo/VoidCommand.h"

VOID_NAMESPACE_OPEN

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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_Type;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
};

class CreateTrackCommand : public VoidUndoCommand
{
public:
    CreateTrackCommand(const SharedPlaybackSequence& sequence, const Sequence::TrackType& type, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::weak_ptr<PlaybackSequence> m_Sequence;
    Sequence::TrackType m_Type;
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
    Sequence::TrackType m_Type;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
};

class RazorTrackCommand : public VoidUndoCommand
{
public:
    RazorTrackCommand(const SharedPlaybackTrack& track, v_frame_t frame, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    PlaybackSequence* m_Sequence;
    Sequence::TrackType m_Type;
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
    Sequence::TrackType m_Type;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
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
    Sequence::TrackType m_TrackType;
    int m_TrackIndex;
    int m_ItemIndex;
    int m_Handle;
};

VOID_NAMESPACE_CLOSE
