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
    int y = Sequencer::TrackSpacing;
    for (int i = 0; i < index; ++i)
        y -= (VideoTrackHeight(i) + Sequencer::TrackSpacing);

    return QRect(0, y, Sequencer::SceneWidth, VideoTrackHeight(index));
}

QRectF STimelineGeometry::TrackHeaderRect(int index) const
{
    int y = TimelineTop();
    for (int i = 0; i < index; ++i)
        y += (VideoTrackHeight(i) + Sequencer::TrackSpacing);

    return QRect(0, y, Sequencer::TrackHeaderWidth, VideoTrackHeight(index));
}

int STimelineGeometry::TrackTop(int index, const Sequence::Type& type)
{
    return type == Sequence::Type::AUDIO ? AudioTrackTop(index) : VideoTrackTop(index);
}

int STimelineGeometry::AudioTrackTop(int index)
{
    return index * (Sequencer::TrackHeight + Sequencer::TrackSpacing);
}

int STimelineGeometry::VideoTrackTop(int index)
{
    int y = -Sequencer::TrackSpacing;
    for (int i = 0; i <= index; ++i)
        y -= (VideoTrackHeight(i) + Sequencer::TrackSpacing);

    return y;
}

int STimelineGeometry::VideoTrackHeight(int index) const
{
    SharedPlaybackTrack track = m_Sequence->VideoTrackAt(index);
    return track->IsEffectsTrack()
            ? track->MaxEffects() * Sequencer::TimelineEffectHeight + 4
            : Sequencer::TrackHeight + track->MaxEffects() * Sequencer::TimelineEffectHeight;
}

int STimelineGeometry::AudioTrackHeight(int index) const
{
    return Sequencer::TrackHeight;
}

int STimelineGeometry::VideoTracksHeight() const
{
    if (m_Sequence)
    {
        int h = 0;
        for (int i = 0; i < m_Sequence->NumVideoTracks(); ++i)
            h += (VideoTrackHeight(i) + Sequencer::TrackSpacing);

        return h;
    }

    return Sequencer::MinSectionHeight;
}

int STimelineGeometry::AudioTracksHeight() const
{
    return m_Sequence ? m_Sequence->NumAudioTracks() * (Sequencer::TrackHeight + Sequencer::TrackSpacing) : Sequencer::MinSectionHeight;
}

int STimelineGeometry::VideoSectionHeight() const
{
    const int h = Sequencer::RulerHeight + VideoTracksHeight();
    return h < Sequencer::MinSectionHeight ? Sequencer::MinSectionHeight : h;
}

int STimelineGeometry::AudioSectionHeight() const
{
    const int h = Sequencer::RulerHeight + AudioTracksHeight();
    return h < Sequencer::MinSectionHeight ? Sequencer::MinSectionHeight : h;
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
