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
#include <QWidget>

/* Internal */
#include "QDefinition.h"
#include "Timeslider.h"
#include "TimelineElements.h"
#include "VoidUi/QExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration for PlayerWidget class */
class Player;

class Timeline : public QWidget
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

	/* Getters */
	inline double Framerate() const { return m_FramerateBox->Framerate(); }
	inline int Frame() const { return m_Timeslider->value(); }

	/**
	 * The range from the Timeslider
	 * Considers any user defined min and max frames
	 */
	// inline int Minimum() const { return m_Timeslider->minimum(); }
	inline int Minimum() const { return m_Timeslider->Minimum(); }
	// inline int Maximum() const { return m_Timeslider->maximum(); }
	inline int Maximum() const { return m_Timeslider->Maximum(); }

	/* Setters */
	inline void SetFramerate(const double rate) { m_FramerateBox->SetFramerate(rate); }
	inline void SetFramerate(const std::string& rate) { m_FramerateBox->SetFramerate(rate); }

	void SetFrame(const int frame);

	inline void SetMaximum(const int frame) { m_Timeslider->setMaximum(frame); }
	inline void SetMinimum(const int frame) { m_Timeslider->setMinimum(frame); }

	void SetUserFirstframe(int frame);
	void SetUserEndframe(int frame);

	void SetRange(const int min, const int max);

	void ResetRange();

	/**
	 * Mark Cached frames on the Timeslider visually
	 */
	inline void AddCacheFrame(int frame) { m_Timeslider->AddCacheFrame(frame); }
	inline void ClearCachedFrames() { m_Timeslider->ClearCachedFrames(); }

	/**
	 * Mark Annotations on the Timeslider visually
	 */
	inline void AddAnnotatedFrame(int frame) { m_Timeslider->AddAnnotatedFrame(frame); }
	inline void RemoveAnnotatedFrame(int frame) { m_Timeslider->RemoveAnnotatedFrame(frame); }
	inline void ClearAnnotatedFrames() { m_Timeslider->ClearAnnotatedFrames(); }

signals:
	void Played(const PlayState& type = PlayState::FORWARDS);
	void PlayedForwards();
	void PlayedBackwards();
	void TimeChanged(int time);
	void fullscreenRequested();

private: /* Members */
	/* Main Layout */
	QVBoxLayout* m_Layout;

	/**
	 * To Keep the Time Display at the center
	 * We keep Left layout Holding Left side elements
	 * Right Layout holding right side elements
	 */
	QHBoxLayout* m_LeftLayout;
	QHBoxLayout* m_RightLayout;

	QPushButton* m_ForwardButton;
	QPushButton* m_NextFrameButton;
	QPushButton* m_EndFrameButton;

	QPushButton* m_BackwardButton;
	QPushButton* m_PrevFrameButton;
	QPushButton* m_StartFrameButton;

	/* Sets the user defined in and out */
	ToggleStatePushButton* m_InFrameButton;
	ToggleStatePushButton* m_OutFrameButton;

	/* Loop State Button - Indicates the current playback loop type */
	LoopTypeButton* m_LoopTypeButton;

	QPushButton* m_FullscreenButton;

	QPushButton* m_StopButton;

	/* Internal timeslider */
	Timeslider* m_Timeslider;

	TimeDisplay* m_TimeDisplay;
	FramerateBox* m_FramerateBox;

	QDoubleValidator* m_DoubleValidator;

	/**
	 * Timeslider Min - Max
	 * This is additionally maintained as to reduce the amount of overhead when
	 * playing as the next frame constanly check if the current frame is the last or not
	 */
	int m_Start, m_End;

	/* Loop mode for playback */
	LoopType m_LoopType;

	std::future<void> m_Worker;
	std::atomic<bool> m_Playing;
	int m_FrameInterval;

	PlayState m_Playstate;

protected: /* Methods */
	void Build();
	void Connect();
	void Setup();

	void TimeUpdated(const int time);
	void PlayForwards();
	void PlayBackwards();

	void Play(const PlayState& state = PlayState::FORWARDS);
	void Stop();
	void Replay();

	void StartPlayback();
	void PlaybackLoop();

	void NextFrame();
	void PreviousFrame();

	void PlayNextFrame();
	void PlayPreviousFrame();

	inline void MoveToStart() { m_Timeslider->setValue(m_Timeslider->Minimum()); }
	inline void MoveToEnd() { m_Timeslider->setValue(m_Timeslider->Maximum()); }

	/**
	 * (Re)sets the In and out framing of the Timeslider
	 * Calling it once sets the frame as in/out frame (User-In/User-Out)
	 * Calling it the next time on the same frame Resets the in/out frame (User-In/User-Out)
	 */
	void ResetInFrame();
	void ResetOutFrame();

	/* Friendly classes */
	friend class Player;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMELINE_H
