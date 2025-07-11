#ifndef _VOID_TIMELINE_ELEMENTS_H
#define _VOID_TIMELINE_ELEMENTS_H

/* Qt */
#include <QPushButton>
#include <QLabel>

/* Internal */
#include "Definition.h"

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
	PlayOnce,
	PingPong
};

struct LoopState
{
	std::string text, icon;	
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
	inline void Update() { setIcon(QIcon(m_LoopState.at(m_LoopType).icon.c_str())); }

};

class TimeDisplay : public QLabel
{
public:
	TimeDisplay(QWidget* parent = nullptr);

private:
	/* Setup the UI */
	void Setup();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMELINE_ELEMENTS_H
