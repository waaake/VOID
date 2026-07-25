// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TIMELINE_GEOMETRY_H
#define _SEQUENCER_TIMELINE_GEOMETRY_H

/* Qt */
#include <QRectF>

/* Internal */
#include "Definition.h"
#include "SDescriptors.h"

VOID_NAMESPACE_OPEN

class STimelineGeometry
{
public:
    QRectF RulerRect() const;
    QRectF HeaderRect() const;
    QRectF TrackRect(int index) const;
    QRectF TrackHeaderRect(int index) const;

    int TimelineTop() const { return Sequencer::RulerHeight; }
    // int ContentLeft() const { return Sequencer::TrackHeaderWidth; }
    int ContentLeft() const { return 0; }

    double FrameToX(v_frame_t frame) const { return frame * m_PixelsPerFrame; }
    v_frame_t XToFrame(double x) const { return static_cast<v_frame_t>(x / m_PixelsPerFrame); }

    double FrameToSceneX(v_frame_t frame) const;
    v_frame_t SceneXToFrame(double x) const;

    double PixelsPerFrame() const { return m_PixelsPerFrame; }
    void SetPixelsPerFrame(double p) { if (p > 0) m_PixelsPerFrame = p; }
    void ResetPixelsPerFrame() { m_PixelsPerFrame = 2.0; }

private:
    double m_PixelsPerFrame = 2.0;
};


VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TIMELINE_GEOMETRY_H
