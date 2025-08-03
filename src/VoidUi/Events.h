// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_EVENTS_H
#define _VOID_EVENTS_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VoidEvents : public QObject
{
	Q_OBJECT

signals:
	void TimeChanged(int time);
	void PlayedForwards();
	void PlayedBackwards();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_EVENTS_H
