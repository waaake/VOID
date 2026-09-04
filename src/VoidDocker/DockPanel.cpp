// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "DockPanel.h"
#include "Docker.h"

VOID_NAMESPACE_OPEN

DockPanel::DockPanel(QWidget* widget, QWidget* parent)
	: QFrame(parent)
	, m_Widget(widget)
	, m_Parent(dynamic_cast<DockWidget*>(parent))
{
	Build();
}

DockPanel::~DockPanel()
{
}

void DockPanel::SetTabText(const QString& text)
{
	if (m_Parent)
		m_Parent->setTabText(m_Parent->indexOf(this), text);
}

void DockPanel::Build()
{
	m_Layout = new QHBoxLayout(this);
	m_Layout->setContentsMargins(0, 0, 0, 0);
	// Reset the widget parent to be this, so that we can perform any operations on this widget probably later
	m_Widget->setParent(this);
	m_Layout->addWidget(m_Widget);
}

VOID_NAMESPACE_CLOSE
