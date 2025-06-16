#ifndef _VOID_TIMELINE_H
#define _VOID_TIMELINE_H

/* STD */
#include <vector>

/* Qt */
#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QComboBox>
#include <QTimer>

/* Internal */
#include "QDefinition.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration for PlayerWidget class */
class Player;

class Timeslider : public QSlider
{
public:
	Timeslider(Qt::Orientation orientation, QWidget *parent = nullptr);

	virtual ~Timeslider();

	/**
	 * Update the range for playback in the timeslider
	 */
	void SetUserFirstframe(int frame);
	void SetUserEndframe(int frame);

	void ResetStartFrame();
	void ResetEndFrame();
	void ResetRange();

	int Minimum();
	int Maximum();

	void AddCacheFrame(int frame);
	void ClearCachedFrames();

	/**
	 * Checks whether a given frame is in Playable range
	 */
	inline bool InRange(int frame) { return frame >= Minimum() && frame <= Maximum(); }

protected:
	void enterEvent(EnterEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;

private: /* Methods */
	void UpdateHovered(int xpos);

private: /* Members */
	bool m_Focussed;
	int m_HovXPos;
	int m_HoveredFrame;

	/* Stores any frame that have been marked as Cached for the timeslider */
	std::vector<int> m_CachedFrames;

	/**
	 * By default the timeline has a minimum and maximum value
	 *
	 * 		|----------------------------|
	 *		100						   200
	 *
	 * And the user can still specify an in-out within the default timeline
	 *
	 *              30          60
	 * 		|-------[------------]-------|
	 *		100						   200
	 *
	 * And when the playback happens, it happens over the user specified range i.e. 30 - 60
	 */

	/* The User set Minimum Frame for playback */
	int m_UserStartframe;
	/* The User set Maximum Frame for playback */
	int m_UserEndframe;

	/* Friends */
	friend class Timeline;
};

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

	/**
	 * A play loop defines what happens when we reach the end of the timeline/media/clip/sequence
	 * Should we continue from beginning? LoopInfinitely
	 * Should we stop? PlayOnce
	 * Should we start going backwards if we're at the end and go forwards once we're in the beginning? PingPong
	 */
	enum class LoopType : short
	{
		LoopInfinitely,
		PlayOnce,
		PingPong
	};

private: /* Members */
	QPushButton* m_ForwardButton;
	QPushButton* m_NextFrameButton;
	QPushButton* m_EndFrameButton;

	QPushButton* m_BackwardButton;
	QPushButton* m_PrevFrameButton;
	QPushButton* m_StartFrameButton;

	/* Sets the user defined in and out */
	QPushButton* m_InFrameButton;
	QPushButton* m_OutFrameButton;

	QPushButton* m_StopButton;

	/* Internal timeslider */
	Timeslider* m_Timeslider;

	QLineEdit* m_TimeDisplay;
	QComboBox* m_FramerateBox;

	QDoubleValidator* m_DoubleValidator;

	QTimer* m_ForwardsTimer;
	QTimer* m_BackwardsTimer;

	/**
	 * Timeslider Min - Max
	 * This is additionally maintained as to reduce the amount of overhead when
	 * playing as the next frame constanly check if the current frame is the last or not
	 */
	int m_Start, m_End;

	/* Loop mode for playback */
	LoopType m_LoopType;

public:
	Timeline(QWidget* parent = nullptr);
	virtual ~Timeline();

	/* Getters */
	inline double Framerate() const { return m_FramerateBox->currentText().toDouble(); }
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
	void SetFramerate(const double rate);
	void SetFramerate(const std::string& rate);

	void SetFrame(const int frame);

	inline void SetMaximum(const int frame) { m_Timeslider->setMaximum(frame); }
	inline void SetMinimum(const int frame) { m_Timeslider->setMinimum(frame); }

	void SetUserFirstframe(int frame);
	void SetUserEndframe(int frame);

	void SetRange(const int min, const int max);

	void ResetRange();

	inline void AddCacheFrame(int frame) { m_Timeslider->AddCacheFrame(frame); }
	inline void ClearCachedFrames() { m_Timeslider->ClearCachedFrames(); }

	/* Friendly classes */
	friend class Player;

protected: /* Methods */
	void Build();
	void Connect();
	void Setup();

	void TimeUpdated(const int time);
	void PlayForwards();
	void PlayBackwards();

	void Play(const PlayState& state = PlayState::FORWARDS);
	void Stop();

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

signals:
	void Played(const PlayState& type = PlayState::FORWARDS);
	void PlayedForwards();
	void PlayedBackwards();
	void TimeChanged(int time);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMELINE_H
