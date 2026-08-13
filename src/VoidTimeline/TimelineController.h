// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TIMELINE_CONTROLLER_H
#define _TIMELINE_CONTROLLER_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "Timeline.h"

VOID_NAMESPACE_OPEN

class VOID_API TimelineController : public QObject
{
    Q_OBJECT
public:
    explicit TimelineController(QObject* parent = nullptr);
    ~TimelineController();

    // Returns the Timeline widget for display and interaction on the player
    Timeline* TimelineWidget() { return m_Timeline; }

    inline int Frame() const { return m_Timeline->Frame(); }

    // Play Controls

    inline void PlayForwards() { m_Timeline->Play(Timeline::PlayState::FORWARDS); }
    inline void PlayBackwards() { m_Timeline->Play(Timeline::PlayState::BACKWARDS); }
    inline void Stop() { m_Timeline->Stop(); }

    // Move to Frame

    inline void NextFrame() { m_Timeline->NextFrame(); }
    inline void PreviousFrame() { m_Timeline->PreviousFrame(); }
    inline void MoveToStart() { m_Timeline->MoveToStart(); }
    inline void MoveToEnd() { m_Timeline->MoveToEnd(); }

    // Clear any markings on the timeline and reset the range 0 - 1

    inline void Clear() { m_Timeline->Clear(); }

    // Setters

    inline void SetFrame(v_frame_t frame) { m_Timeline->SetFrame(frame); }
    inline void SetUserFirstframe(int frame) { m_Timeline->SetUserFirstframe(frame); }
    inline void SetUserEndframe(int frame) { m_Timeline->SetUserEndframe(frame); }
    inline void SetRange(int start, int end) { m_Timeline->SetRange(start, end); }

    // Framerate

    inline double Framerate() const { return m_Timeline->Framerate(); }
    inline void EditFramerate() { m_Timeline->EditFramerate(); }

    // /**
    //  * (Re)sets the In and out framing of the Timeslider
    //  * Calling it once sets the frame as in/out frame (User-In/User-Out)
    //  * Calling it the next time on the same frame Resets the in/out frame (User-In/User-Out)
    //  */
    inline void ResetInFrame() { m_Timeline->ResetInFrame(); }
    inline void ResetOutFrame() { m_Timeline->ResetOutFrame(); }
    inline void ResetRange() { m_Timeline->ResetRange(); }

    // Markings

    void MarkAnnotated(int frame) { m_Timeline->AddAnnotatedFrame(frame); }
    void MarkAnnotated(const std::vector<int>& frames) { m_Timeline->SetAnnotatedFrames(frames); }
	void MarkAnnotated(std::vector<int>&& frames) { m_Timeline->SetAnnotatedFrames(frames); }
    void RemoveAnnotated(int frame) { m_Timeline->RemoveAnnotatedFrame(frame); }

    void MarkCached(int frame) { m_Timeline->AddCacheFrame(frame); }
    void RemoveCached(int frame) { m_Timeline->RemoveCachedFrame(frame); }
    void ClearCached() { m_Timeline->ClearCachedFrames(); }

signals:
    void timeChanged(int);
	void fullscreenRequested();
	void playbackStateChanged(const Timeline::PlayState&);
	void mediaFinished(const Timeline::PlayState& type);
	void seeked(v_frame_t);

private:
    Timeline* m_Timeline;
};

VOID_NAMESPACE_CLOSE

#endif // _TIMELINE_CONTROLLER_H
