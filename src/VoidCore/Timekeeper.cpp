// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cmath>

/* Internal */
#include "Timekeeper.h"

VOID_NAMESPACE_OPEN

Timekeeper::Timekeeper()
    : m_CurrentFrame(-1)
    , m_CurrentTime(-1.0)
    , m_Mediarate(24.0)
    , m_FrameDisplay(FrameDisplayMode::FRAMES)
{
}

Timekeeper::~Timekeeper()
{
}

Timekeeper& Timekeeper::Instance()
{
    static Timekeeper instance;
    return instance;
}

void Timekeeper::SetRange(v_frame_t start, v_frame_t end)
{
    m_Frange.startframe = start;
    m_Frange.endframe = end;
    Reset();
}

void Timekeeper::Reset()
{
    m_CurrentFrame = m_Frange.startframe;
    m_CurrentTime = -1.0;
}

void Timekeeper::SetFrame(v_frame_t frame)
{
    /**
     * When the frame is set, it needs to also reset the current time
     * as this event probably happened by a user-call or user-click in order
     * to seek to a given time in the slider
     */
    m_CurrentFrame = frame;
    if (m_CurrentTime >= 0)
        m_CurrentTime = (double)m_CurrentFrame / m_Frange.framerate;
}

std::string Timekeeper::DisplayStart() const
{
    return m_FrameDisplay == FrameDisplayMode::FRAMES ? std::to_string(m_Frange.startframe) : m_Frange.StartTC();
}

std::string Timekeeper::DisplayEnd() const
{
    return m_FrameDisplay == FrameDisplayMode::FRAMES ? std::to_string(m_Frange.endframe) : m_Frange.EndTC();
}

std::string Timekeeper::DisplayFrame(v_frame_t frame) const
{
    return m_FrameDisplay == FrameDisplayMode::FRAMES ? std::to_string(frame) : Timecode(frame, m_Mediarate).String();
}


v_frame_t Timekeeper::NextFrame()
{
    /**
     * We can't really ever sync to audio when the playback rate of video is different
     * than the actual rate of the media and the audio, don't even bother to continue here
     */
    if (m_CurrentTime < 0 || HasDifferentRate())
        return NextFrame__();

    int distance = m_CurrentFrame - ConvertedTime();
    /**
     * The time is lagging behind --
     * For now return the Next Frame as supposed to
     * with the understanding that the time will catch up
     */
    if (std::abs(distance) < 3)
        return NextFrame__();

    // -ve Distance: time (from the audio) is ahead than the threshold
    if (distance < 0)
    {
        // Catch up to it slowly
        m_CurrentFrame += std::abs(distance) / 2;

        if (m_CurrentFrame > m_Frange.endframe)
            m_CurrentFrame = m_Frange.startframe;

        return m_CurrentFrame;
    }

    // Time (from the audio) is trailing behind at the moment, return the frame without increment
    // This basically mimics slowing down the video fps to catch-up to the audio
    return m_CurrentFrame;
}

v_frame_t Timekeeper::NextFrame(int offset)
{
    /**
     * We can't really ever sync to audio when the playback rate of video is different
     * than the actual rate of the media and the audio, don't even bother to continue here
     */
    if (m_CurrentTime < 0 || HasDifferentRate())
        return NextFrame__(offset);

    int distance = m_CurrentFrame - ConvertedTime();
    /**
     * The time is lagging behind --
     * For now return the Next Frame as supposed to
     * with the understanding that the time will catch up
     */
    if (std::abs(distance) < 3)
        return NextFrame__();

    // -ve Distance: time (from the audio) is ahead than the threshold
    if (distance < 0)
    {
        // Catch up to it slowly
        m_CurrentFrame += std::abs(distance) / 2;

        if (m_CurrentFrame > m_Frange.endframe)
            m_CurrentFrame = m_Frange.startframe;

        return m_CurrentFrame;
    }

    // Time (from the audio) is trailing behind at the moment, return the frame without increment
    // This basically mimics slowing down the video fps to catch-up to the audio
    return m_CurrentFrame;
}

v_frame_t Timekeeper::PreviousFrame()
{
    /**
     * We can't really ever sync to audio when the playback rate of video is different
     * than the actual rate of the media and the audio, don't even bother to continue here
     */
    if (m_CurrentTime < 0 || HasDifferentRate())
        return PreviousFrame__();

    int distance = m_CurrentFrame - ConvertedTime();
    /**
     * The time is lagging behind --
     * For now return the Next Frame as supposed to
     * with the understanding that the time will catch up
     */
    if (std::abs(distance) < 3)
        return PreviousFrame__();

    // Time (from the audio) is trailing behind at the moment, return the frame without increment
    if (distance > 0)
    {
        // Catch up to it slowly
        m_CurrentFrame += std::abs(distance) / 2;

        if (m_CurrentFrame < m_Frange.startframe)
            m_CurrentFrame = m_Frange.endframe;

        return m_CurrentFrame;
    }

    // +ve Distance: time (from the audio) is ahead than the threshold
    // This basically mimics slowing down the video fps to catch-up to the audio
    return m_CurrentFrame;
}

v_frame_t Timekeeper::PreviousFrame(int offset)
{
    /**
     * We can't really ever sync to audio when the playback rate of video is different
     * than the actual rate of the media and the audio, don't even bother to continue here
     */
    if (m_CurrentTime < 0 || HasDifferentRate())
        return PreviousFrame__(offset);

    int distance = m_CurrentFrame - ConvertedTime();
    /**
     * The time is lagging behind --
     * For now return the Next Frame as supposed to
     * with the understanding that the time will catch up
     */
    if (std::abs(distance) < 3)
        return PreviousFrame__();

    // Time (from the audio) is trailing behind at the moment, return the frame without increment
    if (distance > 0)
    {
        // Catch up to it slowly
        m_CurrentFrame += std::abs(distance) / 2;

        if (m_CurrentFrame < m_Frange.startframe)
            m_CurrentFrame = m_Frange.endframe;

        return m_CurrentFrame;
    }

    // +ve Distance: time (from the audio) is ahead than the threshold
    // This basically mimics slowing down the video fps to catch-up to the audio
    return m_CurrentFrame;
}

v_frame_t Timekeeper::NextFrame__()
{
    if (m_CurrentFrame >= m_Frange.endframe)
        m_CurrentFrame = m_Frange.startframe;
    else
        m_CurrentFrame++;
    return m_CurrentFrame;
}

v_frame_t Timekeeper::NextFrame__(int offset)
{
    if (m_CurrentFrame + offset > m_Frange.endframe)
        m_CurrentFrame = m_CurrentFrame == m_Frange.endframe ? m_Frange.startframe : m_Frange.endframe;
    else
        m_CurrentFrame += offset;

    return m_CurrentFrame;
}

v_frame_t Timekeeper::PreviousFrame__()
{
    if (m_CurrentFrame <= m_Frange.startframe)
        m_CurrentFrame = m_Frange.endframe;
    else
        m_CurrentFrame--;
    return m_CurrentFrame;
}

v_frame_t Timekeeper::PreviousFrame__(int offset)
{
    if (m_CurrentFrame - offset < m_Frange.startframe)
        m_CurrentFrame = m_CurrentFrame == m_Frange.startframe ? m_Frange.endframe : m_Frange.startframe;
    else
        m_CurrentFrame -= offset;
    return m_CurrentFrame;
}

VOID_NAMESPACE_CLOSE
