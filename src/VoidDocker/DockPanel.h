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

class DockWidget;

class DockPanel : public QFrame
{
public:
	DockPanel(QWidget* widget, QWidget* parent = nullptr);
	~DockPanel();

	inline QWidget* Widget() const { return m_Widget; }
	inline int PanelId() const { return m_Widget ? m_Widget->objectName().toInt() : -1; }
	void VOID_API SetTabText(const QString& text);

private: /* Members */
	QWidget* m_Widget;
	DockWidget* m_Parent;
	QHBoxLayout* m_Layout;

private: /* Methods */
	void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCKER_PANEL_H
