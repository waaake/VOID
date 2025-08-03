// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_DOCKER_PANEL_H
#define _VOID_DOCKER_PANEL_H

/* Qt */
#include <QFrame>
#include <QLayout>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class DockPanel : public QFrame
{
public:
	DockPanel(QWidget* widget, QWidget* parent = nullptr);
	~DockPanel();

private: /* Members */
	QWidget* widget;
	QHBoxLayout* m_Layout;

private: /* Methods */
	void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCKER_PANEL_H
