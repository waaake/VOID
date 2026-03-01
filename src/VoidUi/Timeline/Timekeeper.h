// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TIMEKEEPER_H
#define _TIMEKEEPER_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class Timekeeper
{
    Timekeeper();
public:
    static Timekeeper& Instance();
    ~Timekeeper();

    Timekeeper(const Timekeeper&) = delete;
    Timekeeper(Timekeeper&&) = delete;

    Timekeeper& operator=(const Timekeeper&) = delete;
    Timekeeper& operator=(Timekeeper&&) = delete;

    void SetStart(v_frame_t start) { m_Start = start; }
    void SetEnd(v_frame_t end) { m_End = end; }
    void SetRange(v_frame_t start, v_frame_t end) { m_Start = start; m_End = end; }

    void SetFramerate(double framerate) { m_Framerate = framerate; }

    // void SetFrame(v_frame_t frame) { m_CurrentFrame = frame; }
    void SetFrame(v_frame_t frame);
    void SetTime(double time) { m_CurrentTime = time; }

    v_frame_t StartFrame() const { return m_Start; }
    v_frame_t EndFrame() const { return m_End; }

    double CurrentTime() const { return m_CurrentTime; }
    v_frame_t CurrentFrame() const { return m_CurrentFrame; }

    v_frame_t NextFrame();
    v_frame_t PreviousFrame();

private: /* Members */
    v_frame_t m_Start, m_End, m_CurrentFrame;
    double m_CurrentTime;
    double m_Framerate;

private: /* Methods */
    inline v_frame_t ConvertedTime() const { return m_CurrentTime * m_Framerate; }
    v_frame_t NextFrame__();
    v_frame_t PreviousFrame__();
};

VOID_NAMESPACE_CLOSE

#endif // _TIMEKEEPER_H
