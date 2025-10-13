// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TIMELINE_ELEMENTS_H
#define _VOID_TIMELINE_ELEMENTS_H

/* Qt */
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "VoidUi/Engine/IconForge.h"

VOID_NAMESPACE_OPEN

/**
 * A play loop defines what happens when we reach the end of the timeline/media/clip/sequence
 * Should we continue from beginning? LoopInfinitely
 * Should we stop? PlayOnce
 * Should we start going backwards if we're at the end and go forwards once we're in the beginning? PingPong
 */
enum class LoopType : short
{
	LoopInfinitely,
	LoopOneInfinitely,
	PlayOnce,
	PingPong
};

struct LoopState
{
	std::string text;
	IconType icon;
};

class LoopTypeButton : public QPushButton
{
	Q_OBJECT

public:
	LoopTypeButton(QWidget* parent = nullptr);
	~LoopTypeButton();

	/**
	 * Set the Loop Type for playback
	 */
	void SetLoopType(const LoopType& looptype);

signals:
	void loopTypeChanged(const LoopType&);

private: /* Members */
	LoopType m_LoopType;
	std::unordered_map<LoopType, LoopState> m_LoopState;

	QMenu* m_Menu;

private: /* Methods */
	void Build();
	void Update();
};

class TimeDisplay : public QLabel
{
public:
	TimeDisplay(QWidget* parent = nullptr);

private:
	/* Setup the UI */
	void Setup();

};

class FramerateBox : public QComboBox
{
	Q_OBJECT

public:
	FramerateBox(QWidget* parent = nullptr);

	inline double Framerate() const { return currentText().toDouble(); }

	inline void SetFramerate(const double rate) { setCurrentText(std::to_string(rate).c_str()); }
	inline void SetFramerate(const std::string& rate) { setCurrentText(rate.c_str()); }

signals:
	void framerateChanged(double);

private: /* Members */
	QDoubleValidator* m_DoubleValidator;

private: /* Methods */
	void Setup();
	void RateChanged(const QString& text);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMELINE_ELEMENTS_H
