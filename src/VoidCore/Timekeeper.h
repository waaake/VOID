// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TIMEKEEPER_H
#define _TIMEKEEPER_H

/* Internal */
#include "Definition.h"
#include "FrameRange.h"

VOID_NAMESPACE_OPEN

enum class FrameDisplayMode
{
    TIMECODE,
    FRAMES
};

/**
 * @brief As the name suggests, keeps time for the player.
 * Keeps a track of what frame is being played right now and what's the next/previous frame
 * that's going to be played.
 * Also allows the audio or the primary stream to set time in seconds, which when playing
 * ensures that the next frame to be rendered out is always around that time. This is done
 * to sync the content around the primary stream of data.
 */
class VOID_API Timekeeper
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
    void SetFramerate(double framerate) { m_Frange.framerate = framerate; }
    double Framerate() const { return m_Frange.framerate; }

    /**
     * @brief Set the current framerate of the media that is being played
     * 
     * @param framerate Rate of playback of the active media.
     */
    void SetMediaFramerate(double framerate) { m_Mediarate = framerate; }
    double MediaFramerate() const { return m_Mediarate; }

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

    inline void SetStart(v_frame_t start) { m_Frange.startframe = start; }
    inline void SetEnd(v_frame_t end) { m_Frange.endframe = end; }
    void SetRange(v_frame_t start, v_frame_t end);

    /**
     * @brief Resets the start time and frame internally to the start
     * 
     */
    void Reset();

    inline v_frame_t StartFrame() const { return m_Frange.startframe; }
    inline v_frame_t EndFrame() const { return m_Frange.endframe; }
    inline v_frame_t CurrentFrame() const { return m_CurrentFrame; }

    inline double CurrentTime() const { return m_CurrentTime; }

    // Timecode
    std::string StartTC(bool dropframe = false) const { return m_Frange.StartTC(dropframe); }
    std::string EndTC(bool dropframe = false) const { return m_Frange.EndTC(dropframe); }
    std::string TC(v_frame_t frame, bool dropframe = false) const { return m_Frange.TC(frame, dropframe); }

    // Display
    std::string DisplayStart() const;
    std::string DisplayEnd() const;
    std::string DisplayFrame(v_frame_t frame) const;

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
    v_frame_t NextFrame(int offset);

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
    v_frame_t PreviousFrame(int offset);

private: /* Members */
    MFrameRange m_Frange;
    v_frame_t m_CurrentFrame;
    double m_CurrentTime;
    double m_Mediarate;
    FrameDisplayMode m_FrameDisplay;

private: /* Methods */
    inline v_frame_t ConvertedTime() const { return static_cast<v_frame_t>(m_CurrentTime * m_Frange.framerate); }
    inline bool HasDifferentRate() const { return m_Mediarate != m_Frange.framerate; }
    v_frame_t NextFrame__();
    v_frame_t NextFrame__(int offset);
    v_frame_t PreviousFrame__();
    v_frame_t PreviousFrame__(int offset);
};

VOID_NAMESPACE_CLOSE

#endif // _TIMEKEEPER_H
