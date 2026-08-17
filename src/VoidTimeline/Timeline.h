// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TIMELINE_H
#define _VOID_TIMELINE_H

/* STD */
#include <atomic>
#include <future>
#include <thread>

/* Qt */
#include <QComboBox>
#include <QLayout>
#include <QPushButton>
#include <QTimer>
#include <QElapsedTimer>
#include <QWidget>

/* Internal */
#include "QDefinition.h"
#include "Timeslider.h"
#include "TimelineElements.h"
#include "VoidQExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class VOID_API Timeline : public QWidget
{
	Q_OBJECT
public:
	enum class PlayState : short
	{
		STOPPED,
		FORWARDS,
		BACKWARDS
	};

public:
	Timeline(QWidget* parent = nullptr);
	virtual ~Timeline();

	void EditFramerate() { m_FramerateBox->setFocus(Qt::FocusReason::TabFocusReason); }

	/* Getters */
	inline double Framerate() const { return m_FramerateBox->Framerate(); }
	inline int Frame() const { return m_Timeslider->value(); }

	/**
	 * The range from the Timeslider
	 * Considers any user defined min and max frames
	 */
	inline int Minimum() const { return m_Timeslider->Minimum(); }
	inline int Maximum() const { return m_Timeslider->Maximum(); }

	/* Setters */
	inline void SetFramerate(const double rate) { m_FramerateBox->SetFramerate(rate); }
	inline void SetFramerate(const std::string& rate) { m_FramerateBox->SetFramerate(rate); }

	void Play(const PlayState& state = PlayState::FORWARDS);
	void Stop();

	void SetFrame(const int frame);
	void NextFrame();
	void PreviousFrame();
	inline void MoveToStart() { m_Timeslider->setValue(m_Timeslider->Minimum()); }
	inline void MoveToEnd() { m_Timeslider->setValue(m_Timeslider->Maximum()); }

	inline void SetMaximum(const int frame) { m_Timeslider->setMaximum(frame); }
	inline void SetMinimum(const int frame) { m_Timeslider->setMinimum(frame); }

	void SetUserFirstframe(int frame);
	void SetUserEndframe(int frame);

	void SetRange(const int min, const int max);
	/**
	 * (Re)sets the In and out framing of the Timeslider
	 * Calling it once sets the frame as in/out frame (User-In/User-Out)
	 * Calling it the next time on the same frame Resets the in/out frame (User-In/User-Out)
	 */
	void ResetInFrame();
	void ResetOutFrame();
	void ResetRange();

	/**
	 * Mark Cached frames on the Timeslider visually
	 */
	inline void AddCacheFrame(int frame) { m_Timeslider->AddCacheFrame(frame); }
	inline void RemoveCachedFrame(int frame) { m_Timeslider->RemoveCachedFrame(frame); }
	inline void ClearCachedFrames() { m_Timeslider->ClearCachedFrames(); }

	/**
	 * Mark Annotations on the Timeslider visually
	 */
	inline void AddAnnotatedFrame(int frame) { m_Timeslider->AddAnnotatedFrame(frame); }
	inline void RemoveAnnotatedFrame(int frame) { m_Timeslider->RemoveAnnotatedFrame(frame); }
	inline void ClearAnnotatedFrames() { m_Timeslider->ClearAnnotatedFrames(); }
	inline void SetAnnotatedFrames(const std::vector<int>& frames) { m_Timeslider->SetAnnotatedFrames(frames); }
	inline void SetAnnotatedFrames(std::vector<int>&& frames) { m_Timeslider->SetAnnotatedFrames(std::move(frames)); }

	void Clear();

signals:
	void timeChanged(int);
	void fullscreenRequested();
	void playbackStateChanged(const PlayState&);
	void mediaFinished(const PlayState & type = PlayState::FORWARDS);
	void seeked(v_frame_t);

private: /* Members */
	QVBoxLayout* m_Layout;
	QHBoxLayout* m_LeftLayout;
	QHBoxLayout* m_RightLayout;

	QPushButton* m_ForwardButton;
	QPushButton* m_NextFrameButton;
	QPushButton* m_EndFrameButton;
	QPushButton* m_BackwardButton;
	QPushButton* m_PrevFrameButton;
	QPushButton* m_StartFrameButton;

	ToggleStatePushButton* m_InFrameButton;
	ToggleStatePushButton* m_OutFrameButton;
	LoopTypeButton* m_LoopTypeButton;
	QPushButton* m_FullscreenButton;
	QPushButton* m_StopButton;

	Timeslider* m_Timeslider;
	TimeEdit* m_TimeDisplay;
	FramerateBox* m_FramerateBox;

	TimeEdit* m_InTimeEdit;
	TimeEdit* m_OutTimeEdit;

	QDoubleValidator* m_DoubleValidator;
	QTimer m_PlayTimer;
	QElapsedTimer m_ElapsedTimer;

	std::future<void> m_Worker;
	std::atomic<bool> m_Playing;
	int m_FrameInterval;

	LoopType m_LoopType;
	PlayState m_Playstate;

protected: /* Methods */
	void Build();
	void Connect();
	void Setup();
	void TimeUpdated(const int time);
	void PlayForwards();
	void PlayBackwards();
	void Replay();
	void StartPlayback();
	void PlaybackLoop();
	void TimerPlaybackLoop();
	void PlayNextFrame();
	void PlayPreviousFrame();
	int ElapsedFrames();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMELINE_H
