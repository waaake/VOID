// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_CONTEXT_H
#define _SEQUENCER_CONTEXT_H

/* Internal */
#include "Definition.h"
#include "Descriptors.h"
#include "SController.h"
#include "STimelineGeometry.h"
#include "Models/SSelectionModel.h"
#include "Models/SHoverModel.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

enum class SequencerAction
{
    NONE,
    SLIP_CLIP,
    TRIM,
    RAZOR,
    RAZOR_ALL,
    MERGE
};

class SequencerContext
{
public:
    SSelectionModel* SelectionModel() { return &m_Selection; }
    SHoverModel* HoverModel() { return &m_Hover; }
    STimelineGeometry* Geometry() { return &m_Geometry; }
    SequencerController* Controller() { return &m_Controller; }
    TimelineController* TimeController() { return m_Controller.TimeController(); }

    void SetAction(const SequencerAction& action) { m_Action = action; }
    void ResetAction() { m_Action = SequencerAction::NONE; }
    const SequencerAction& Action() const { return m_Action; }
    void SetSequence(const SharedPlaybackSequence& sequence) { m_Sequence = sequence; }
    SharedPlaybackSequence Sequence() const { return m_Sequence; }
    bool HasActiveSequence() const { return (bool)m_Sequence; }

private:
    SSelectionModel m_Selection;
    SHoverModel m_Hover;
    STimelineGeometry m_Geometry;
    SequencerController m_Controller;
    v_frame_t m_CurrentFrame { 0 };
    SequencerAction m_Action { SequencerAction::NONE };
    SharedPlaybackSequence m_Sequence { nullptr };
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CONTEXT_H
