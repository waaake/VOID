// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Timekeeper.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

Timekeeper::Timekeeper()
    : m_Start(0)
    , m_End(1)
    , m_CurrentFrame(-1)
    , m_CurrentTime(-1.0)
    , m_Framerate(24.0)
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

void Timekeeper::Reset()
{
    m_CurrentFrame = -1;
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
    VOID_LOG_INFO("Setting Current Frame: {}", frame);

    if (m_CurrentTime >= 0)
    {
        m_CurrentTime = (double)m_CurrentFrame / m_Framerate;
        VOID_LOG_INFO("Setting Current Time: {}s", m_CurrentTime);
    }
}

v_frame_t Timekeeper::NextFrame()
{
    if (m_CurrentTime < 0)
        return NextFrame__();

    /**
     * The time is lagging behind --
     * For now return the Next Frame as supposed to
     * with the understanding that the time will catch up
     */
    if (ConvertedTime() < m_CurrentFrame)
        return NextFrame__();
    
    v_frame_t time = ConvertedTime();
    if (time >= m_End)
        m_CurrentFrame = m_Start;
    else
        m_CurrentFrame = ++time;
    return m_CurrentFrame;
}

v_frame_t Timekeeper::PreviousFrame()
{
    if (m_CurrentTime < 0)
        return PreviousFrame__();

    /**
     * The time is lagging behind --
     * For now return the Next Frame as supposed to
     * with the understanding that the time will catch up
     */
    if (ConvertedTime() > m_CurrentFrame)
        return PreviousFrame__();
    
    v_frame_t time = ConvertedTime();

    if (time <= m_Start)
        m_CurrentFrame = m_End;
    else
        m_CurrentFrame = --time;
    return m_CurrentFrame;
}

v_frame_t Timekeeper::NextFrame__()
{
    if (m_CurrentFrame >= m_End)
        m_CurrentFrame = m_Start;
    else
        m_CurrentFrame++;
    return m_CurrentFrame;
}

v_frame_t Timekeeper::PreviousFrame__()
{
    if (m_CurrentFrame <= m_Start)
        m_CurrentFrame = m_End;
    else
        m_CurrentFrame--;
    return m_CurrentFrame;
}

VOID_NAMESPACE_CLOSE
