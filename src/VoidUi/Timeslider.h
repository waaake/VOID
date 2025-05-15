#ifndef _VOID_TIMESLIDER_H
#define _VOID_TIMESLIDER_H

/* Qt */
#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QComboBox>
#include <QTimer>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class Timeslider : public QWidget
{
	Q_OBJECT

private: /* Members */
	QPushButton* m_ForwardButton;
	QPushButton* m_NextFrameButton;
	QPushButton* m_EndFrameButton;

	QPushButton* m_BackwardButton;
	QPushButton* m_PrevFrameButton;
	QPushButton* m_StartFrameButton;
				
	QPushButton* m_StopButton;

	QSlider* m_Timeslider;

	QLineEdit* m_TimeDisplay;
	QComboBox* m_FramerateBox;

	QDoubleValidator* m_DoubleValidator;

	// VoidEvents e;

	QTimer* m_ForwardsTimer;
	QTimer* m_BackwardsTimer;

public:
	enum class PlayState : short
	{
		STOPPED,
		FORWARDS,
		BACKWARDS
	};

public:
	Timeslider(QWidget* parent = nullptr);
	virtual ~Timeslider();

	/* Getters */
	double Framerate() const;
	int Frame() const;
	int Minimum() const;
	int Maximum() const;

	/* Setters */
	void SetFramerate(const double rate);
	void SetFramerate(const std::string& rate);

	void SetFrame(const int frame);

	void SetMaximum(const int frame);
	void SetMinimum(const int frame);
	void SetRange(const int min, const int max);

private: /* Methods */
	void Build();
	void Stylize();
	void Connect();
	void Setup();

	void TimeUpdated(const int time);
	void PlayForwards();
	void PlayBackwards();

	void Play(const PlayState& state = PlayState::FORWARDS);
	void Stop();

	void NextFrame();
	void PreviousFrame();

	signals:
	void Played(const PlayState& type = PlayState::FORWARDS);
	void PlayedForwards();
	void PlayedBackwards();
	void TimeChanged(int time);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMESLIDER_H
