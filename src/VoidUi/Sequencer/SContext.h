// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_CONTEXT_H
#define _SEQUENCER_CONTEXT_H

/* Internal */
#include "Definition.h"
#include "SController.h"
#include "SDescriptors.h"
#include "STimelineGeometry.h"
#include "Models/SSelectionModel.h"
#include "Models/SHoverModel.h"

VOID_NAMESPACE_OPEN

class SequencerContext
{
public:
    SSelectionModel* SelectionModel() { return &m_Selection; }
    SHoverModel* HoverModel() { return &m_Hover; }
    STimelineGeometry* Geometry() { return &m_Geometry; }
    SequencerController* Controller() { return &m_Controller; }

    v_frame_t CurrentFrame() const { return m_CurrentFrame; }
    void SetCurrentFrame(v_frame_t frame) { m_CurrentFrame = frame; }

private:
    SSelectionModel m_Selection;
    SHoverModel m_Hover;
    STimelineGeometry m_Geometry;
    SequencerController m_Controller;
    v_frame_t m_CurrentFrame = 0;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CONTEXT_H
