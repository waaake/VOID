// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TIMEKEEPER_H
#define _TIMEKEEPER_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * @brief As the name suggests, keeps time for the player.
 * Keeps a track of what frame is being played right now and what's the next/previous frame
 * that's going to be played.
 * Also allows the audio or the primary stream to set time in seconds, which when playing
 * ensures that the next frame to be rendered out is always around that time. This is done
 * to sync the content around the primary stream of data.
 */
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

    inline void SetStart(v_frame_t start) { m_Start = start; }
    inline void SetEnd(v_frame_t end) { m_End = end; }
    void SetRange(v_frame_t start, v_frame_t end);

    /**
     * @brief Resets the start time and frame internally to the start
     * 
     */
    void Reset();

    inline v_frame_t StartFrame() const { return m_Start; }
    inline v_frame_t EndFrame() const { return m_End; }
    inline v_frame_t CurrentFrame() const { return m_CurrentFrame; }

    inline double CurrentTime() const { return m_CurrentTime; }

    /**
     * @brief Returns the Next Frame based on the current frame and also on the current time
     * if that has been set. If the time is set and is greater than the current frame,
     * then the next frame provided is the converted time (x framerate) + 1.
     * If however, the time is set and is lesser than the current frame, then the next frame is provided
     * as is.
     * 
     * @return v_frame_t The next frame.
     */
    v_frame_t NextFrame();

    /**
     * @brief Returns the Previous Frame based on the current frame and also on the current time
     * if that has been set. If the time is set and is lesser than the current frame,
     * then the next frame provided is the converted time (x framerate) - 1.
     * If however, the time is set and is greater than the current frame, then the previous frame is provided
     * as is.
     * 
     * @return v_frame_t The previous frame.
     */
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
