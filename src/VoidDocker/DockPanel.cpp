// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "DockPanel.h"

VOID_NAMESPACE_OPEN

DockPanel::DockPanel(QWidget* widget, QWidget* parent)
	: QFrame(parent)
	, widget(widget)
{
	Build();
}

DockPanel::~DockPanel()
{
}

void DockPanel::Build()
{
	/* The base layout in which */
	m_Layout = new QHBoxLayout(this);

	/* Adjust the margins */
	m_Layout->setContentsMargins(0, 0, 0, 0);

	/* Add the widget to the layout */
	m_Layout->addWidget(widget);
}

VOID_NAMESPACE_CLOSE
