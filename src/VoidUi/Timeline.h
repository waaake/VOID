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
#include "Definition.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration for PlayerWidget class */
class Player;

class Timeslider : public QSlider
{
public:
	Timeslider(Qt::Orientation orientation, QWidget *parent = nullptr);

	virtual ~Timeslider();

	void AddCacheFrame(int frame);
	void ClearCachedFrames();

protected:
	void enterEvent(QEvent* event) override;
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
};

class Timeline : public QWidget
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

	/* Internal timeslider */
	Timeslider* m_Timeslider;

	QLineEdit* m_TimeDisplay;
	QComboBox* m_FramerateBox;

	QDoubleValidator* m_DoubleValidator;

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
	Timeline(QWidget* parent = nullptr);
	virtual ~Timeline();

	/* Getters */
	inline double Framerate() const { return m_FramerateBox->currentText().toDouble(); }
	inline int Frame() const { return m_Timeslider->value(); }
	inline int Minimum() const { return m_Timeslider->minimum(); }
	inline int Maximum() const { return m_Timeslider->maximum(); }

	/* Setters */
	void SetFramerate(const double rate);
	void SetFramerate(const std::string& rate);

	void SetFrame(const int frame);

	inline void SetMaximum(const int frame) { m_Timeslider->setMaximum(frame); }
	inline void SetMinimum(const int frame) { m_Timeslider->setMinimum(frame); }

	void SetRange(const int min, const int max);

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

	inline void MoveToStart() { m_Timeslider->setValue(m_Timeslider->minimum()); }
	inline void MoveToEnd() { m_Timeslider->setValue(m_Timeslider->maximum()); }

signals:
	void Played(const PlayState& type = PlayState::FORWARDS);
	void PlayedForwards();
	void PlayedBackwards();
	void TimeChanged(int time);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMELINE_H
