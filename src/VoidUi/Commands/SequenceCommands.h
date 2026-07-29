// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidCommand.h"
#include "VoidObjects/Sequence/TrackItem.h"
#include "VoidUi/Sequencer/SController.h"

VOID_NAMESPACE_OPEN

class MoveTrackItemCommand : public VoidUndoCommand
{
public:
    MoveTrackItemCommand(SharedTrackItem& item, v_frame_t frame, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::weak_ptr<TrackItem> m_Item;
    v_frame_t m_Requested, m_Previous;
};

class MoveItemToTrackCommand : public VoidUndoCommand
{
public:
    MoveItemToTrackCommand(SequencerController* controller,
        SharedTrackItem& item,
        int currentTrackIndex,
        int trackIndex,
        v_frame_t frame,
        QUndoCommand* parent = nullptr
    );
    void undo() override;
    bool Redo() override;

private:
    SequencerController* m_Controller;
    std::weak_ptr<TrackItem> m_Item;
    int m_CurrentTrackIndex;
    int m_TrackIndex;
    v_frame_t m_Requested, m_Previous;
};

class SetTrackItemColorCommand : public VoidUndoCommand
{
public:
    SetTrackItemColorCommand(const SharedTrackItem& item, const QColor& color, QUndoCommand* parent = nullptr);
    SetTrackItemColorCommand(const SharedTrackItem& item, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::weak_ptr<TrackItem> m_Item;
    QColor m_Color;
    QColor m_Previous;
    bool m_Reset;
};

class ToggleLockTrackCommand : public VoidUndoCommand
{
public:
    explicit ToggleLockTrackCommand(const SharedPlaybackTrack& track, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::weak_ptr<PlaybackTrack> m_Track;
    bool m_Previous;
};

class ToggleTrackStateCommand : public VoidUndoCommand
{
public:
    explicit ToggleTrackStateCommand(const SharedPlaybackTrack& track, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::weak_ptr<PlaybackTrack> m_Track;
    bool m_Previous;
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
    DeleteTrackCommand(const SharedPlaybackSequence& sequence, int index, const Sequence::TrackType& type, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::weak_ptr<PlaybackSequence> m_Sequence;
    Sequence::TrackType m_Type;
    int m_TrackIndex;
    rapidjson::Value m_TrackData;
};

class DeleteTrackItemCommand : public VoidUndoCommand
{
public:
    DeleteTrackItemCommand(const SharedPlaybackSequence& sequence, const Sequence::TrackType& type, int trackindex, int index, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    std::weak_ptr<PlaybackSequence> m_Sequence;
    Sequence::TrackType m_Type;
    int m_TrackIndex;
    int m_ItemIndex;
    rapidjson::Value m_ItemData;
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

VOID_NAMESPACE_CLOSE
