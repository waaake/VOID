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

    /**
     * @brief Set the current framerate.
     * 
     * @param framerate Rate of playback from the active timeline.
     */
    void SetFramerate(double framerate) { m_Framerate = framerate; }
    // void SetFrame(v_frame_t frame) { m_CurrentFrame = frame; }

    /**
     * @brief Set the current frame. This method is likely to be called when the
     * frame was set mannually. Eventually will result in a seek operation
     * 
     * @param frame Framenumber.
     */
    void SetFrame(v_frame_t frame);
    /**
     * @brief Set the Time in seconds, likely to be invoked by a second stream
     * (audio) to set the current playback duration in time. This is to ensure
     * that the audio playback and the video frame being played are always in sync
     * 
     * @param time Time in seconds.
     */
    void SetTime(double time) { m_CurrentTime = time; }

    void SetStart(v_frame_t start) { m_Start = start; }
    void SetEnd(v_frame_t end) { m_End = end; }
    void SetRange(v_frame_t start, v_frame_t end) { m_Start = start; m_End = end; }

    void Reset();

    v_frame_t StartFrame() const { return m_Start; }
    v_frame_t EndFrame() const { return m_End; }
    v_frame_t CurrentFrame() const { return m_CurrentFrame; }

    double CurrentTime() const { return m_CurrentTime; }

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
