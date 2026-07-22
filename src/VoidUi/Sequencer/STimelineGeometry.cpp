// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "STimelineGeometry.h"

VOID_NAMESPACE_OPEN

QRectF STimelineGeometry::RulerRect() const
{
    return QRectF(ContentLeft(), 0, Sequencer::SceneWidth, Sequencer::RulerHeight);
}

QRectF STimelineGeometry::HeaderRect() const
{
    return QRectF(0, 0, Sequencer::TrackHeaderWidth, Sequencer::SceneHeight);
}

QRectF STimelineGeometry::TrackRect(int index) const
{
    const int y = TimelineTop() + index * (Sequencer::TrackHeight + Sequencer::TrackSpacing);
    return QRectF(ContentLeft(), y, Sequencer::SceneWidth, Sequencer::TrackHeight);
}

QRectF STimelineGeometry::TrackHeaderRect(int index) const
{
    const int y = TimelineTop() + index * (Sequencer::TrackHeight + Sequencer::TrackSpacing);
    return QRectF(0, y, Sequencer::TrackHeaderWidth, Sequencer::TrackHeight);
}

double STimelineGeometry::FrameToSceneX(v_frame_t frame) const
{
    return ContentLeft() + FrameToX(frame);
}

v_frame_t STimelineGeometry::SceneXToFrame(double x) const
{
    return XToFrame(x - ContentLeft());
}

VOID_NAMESPACE_CLOSE
