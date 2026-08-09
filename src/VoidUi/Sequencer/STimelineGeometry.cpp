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
    int y = TimelineTop();
    for (int i = 0; i < index; ++i)
        y += (VideoTrackHeight(i) + Sequencer::TrackSpacing);

    return QRect(0, y, Sequencer::SceneWidth, VideoTrackHeight(index));
}

QRectF STimelineGeometry::TrackHeaderRect(int index) const
{
    int y = TimelineTop();
    for (int i = 0; i < index; ++i)
        y += (VideoTrackHeight(i) + Sequencer::TrackSpacing);

    return QRect(0, y, Sequencer::TrackHeaderWidth, VideoTrackHeight(index));
}

int STimelineGeometry::VideoTrackHeight(int index) const
{
    SharedPlaybackTrack track = m_Sequence->VideoTrackAt(index);
    return Sequencer::TrackHeight + track->MaxEffects() * Sequencer::TimelineEffectHeight;
}

int STimelineGeometry::AudioTrackHeight(int index) const
{
    return Sequencer::TrackHeight;
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
