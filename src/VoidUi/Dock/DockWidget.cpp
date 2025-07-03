/* Qt */
#include <QApplication>
#include <QStyle>
#include <QToolButton>
#include <QMouseEvent>

/* Internal */
#include "DockWidget.h"

VOID_NAMESPACE_OPEN

DockTab::DockTab(QWidget* parent)
	: QTabBar(parent)
{
	/* Sets the tabs as movable */
	setMovable(true);
}

void DockTab::mousePressEvent(QMouseEvent* event)
{
	/* Save the point where the click happened */
	m_StartPos = event->pos();
	/* And enable dragging */
	m_Dragging = true;

	// std::cout << "Pressed" << std::endl;

	QTabBar::mousePressEvent(event);
}

void DockTab::mouseMoveEvent(QMouseEvent* event)
{
	/* If the mouse was held and we're dragging */
	if (m_Dragging)
	{
		/* Check the delta of the movement */
		if ((event->pos() - m_StartPos).manhattanLength() > 200)
		{
			// std::cout << "Tab Detach Requested" << std::endl;
			m_Dragging = false;
			/* Tab Detach Requested */
			emit tabDetachRequested(currentIndex(), mapToGlobal(event->pos()));
		}
	}

	QTabBar::mouseMoveEvent(event);
}

void DockTab::mouseReleaseEvent(QMouseEvent* event)
{
	/* Mouse has been release and so is the dragging */
	m_Dragging = false;

	QTabBar::mouseReleaseEvent(event);
}

DockWidget::DockWidget(QWidget* parent)
	: QTabWidget(parent)
{
	m_DockTab = new DockTab(this);

	setTabBar(m_DockTab);

	connect(m_DockTab, &DockTab::tabDetachRequested, this, &DockWidget::UndockTab);
}

void DockWidget::AddDock(QWidget* widget, const std::string& title, bool closable)
{
	/* Create a dockable panel for the widget */
	DockPanel* panel = new DockPanel(widget, this);
	/* Add that to the tab widget */
	int index = addTab(panel, title.c_str());

	if (closable)
		SetTabClosable(index);
}

void DockWidget::SetTabClosable(int index)
{
	QToolButton* closeButton = new QToolButton();
	closeButton->setIcon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
    closeButton->setFixedSize(10, 10);
	closeButton->setAutoRaise(true);

	connect(closeButton, &QToolButton::clicked, this, [=]() { emit tabCloseRequested(index); });

	/* Set the tool button on the tabbar */
	tabBar()->setTabButton(index, QTabBar::RightSide, closeButton);
}

void DockWidget::CloseTab(int index)
{
	//DockPanel* panel = static_cast<DockPanel*>(widget(index));

	removeTab(index);

	///* Delete the panel that was added to the widget */
	//delete panel;
}

void DockWidget::UndockTab(int index, const QPoint& position)
{
	/* Get the Dock Panel at the index */
	DockPanel* panel = static_cast<DockPanel*>(widget(index));

	/* Unparent the Panel */
	panel->setParent(nullptr);

	/* Remove the tab */
	removeTab(index);

	/* Move the panel to that position */
	panel->show();
	panel->move(position);
}

VOID_NAMESPACE_CLOSE
