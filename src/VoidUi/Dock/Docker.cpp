// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QToolButton>

/* Internal */
#include "Docker.h"
#include "DockManager.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

VoidDocker::VoidDocker(QWidget* parent)
    : QDockWidget(parent)
{
}

VoidDocker::VoidDocker(const std::string& title, QWidget* parent)
    : QDockWidget(title.c_str(), parent)
{
}

VoidDocker::~VoidDocker()
{
}

void VoidDocker::SetClosable(const bool closable)
{
    if (closable)
        setFeatures(features() & QDockWidget::DockWidgetClosable);
    else
        setFeatures(features() & ~QDockWidget::DockWidgetClosable);
}

/* Dock Tab {{{ */

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

/* }}} */

/* Dock Widget {{{ */

DockWidget::DockWidget(DockSplitter* parent)
	: QTabWidget(parent)
	, m_Splitter(parent)
{
	m_DockTab = new DockTab(this);

	setTabBar(m_DockTab);

	/* Dock Panel Options */
	SetupOptions();

	/* Connect Signals */
	Connect();
}

void DockWidget::AddDock(QWidget* widget, const std::string& title, bool closable)
{
	/* Create a dockable panel for the widget */
	DockPanel* panel = new DockPanel(widget, this);
	/* Add that to the tab widget */
	int index = addTab(panel, title.c_str());
	setCurrentIndex(index);

	if (closable)
		SetTabClosable(index);
}

void DockWidget::AddDockManagerWidget(int index)
{
	/* Get the struct at the provided index */
	DockStruct d = DockManager::Instance().Dock(index);

	/* It is an invalid Dock */
	if (d.id < 0)
		return;
	
	/* All Dock Manager Widgets shall be closable */
	AddDock(d.widget, d.name, true);
}

void DockWidget::SetTabClosable(int index)
{
	QToolButton* closeButton = new QToolButton();
	closeButton->setIcon(QIcon(":resources/icons/icon_close.svg"));
    closeButton->setFixedSize(14, 14);
	closeButton->setAutoRaise(true);

	// connect(closeButton, &QToolButton::clicked, this, [=]() { emit tabCloseRequested(index); });
	connect(closeButton, &QToolButton::clicked, this, [=]() { CloseTab(index); });

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

void DockWidget::SetupOptions()
{
	/* Options Button */
	m_PanelOptions = new QToolButton;
	m_PanelOptions->setIcon(QIcon(":resources/icons/icon_tab_options.svg"));
	m_PanelOptions->setAutoRaise(true);

	/* Add to the Tab Widget, just before the tabs start */
	setCornerWidget(m_PanelOptions, Qt::TopLeftCorner);
	/* Minimum Size ensures the corner widget remains visible even when no tabs are */
	cornerWidget(Qt::TopLeftCorner)->setMinimumSize(m_PanelOptions->sizeHint());

	/* Options Menu */
	m_Options = new QMenu(m_PanelOptions);
	m_DockMenu = new QMenu("Docks", m_PanelOptions);

	m_ClosePaneAction = new QAction("Close Pane");
	m_SplitHorizontalAction = new QAction("Split Horizontally");
	m_SplitVerticalAction = new QAction("Split Vertically");
	
	m_Options->addAction(m_ClosePaneAction);
	m_Options->addAction(m_SplitHorizontalAction);
	m_Options->addAction(m_SplitVerticalAction);

	/* ------------------------ */
	m_Options->addSeparator();

	m_Options->addMenu(m_DockMenu);

	/* Setup Dock Menu from available options */
	ResetDockMenu();
}

void DockWidget::Connect()
{
	/* Tab Bar */
	connect(m_DockTab, &DockTab::tabDetachRequested, this, &DockWidget::UndockTab);

	/* Panel Options */
	connect(m_PanelOptions, &QToolButton::clicked, this, [this]() { m_Options->popup(m_PanelOptions->mapToGlobal(m_PanelOptions->rect().center())); });
	connect(m_ClosePaneAction, &QAction::triggered, this, &DockWidget::ClosePane);
	connect(m_SplitHorizontalAction, &QAction::triggered, this, [this]() { emit splitRequested(Qt::Horizontal); });
	connect(m_SplitVerticalAction, &QAction::triggered, this, [this]() { emit splitRequested(Qt::Vertical); });

	/* Dock Manager */
	connect(&DockManager::Instance(), &DockManager::updated, this, &DockWidget::ResetDockMenu);
}

void DockWidget::ClosePane()
{
	/**
	 * Before we request for this panel to be deleted, unparent all of the existing tabs,
	 * so they don't get deleted
	 */
	for (int i = count() - 1; i >= 0; --i)
	{
		widget(i)->setParent(nullptr);
	}

	/* With all tabs unparented, request for this to be removed */
	emit closureRequested(this);
}

void DockWidget::ResetDockMenu()
{
	/* Clear all of the Dock Menu */
	m_DockMenu->clear();

	/* From the Available docks in the Dock Manager, setup Menu Actions */
	for (const std::pair<int, DockStruct>& it: DockManager::Instance().AvailableDocks())
	{
		QAction* action = new QAction(it.second.name.c_str());
		connect(action, &QAction::triggered, this, [=]() { AddDockManagerWidget(it.first); });

		m_DockMenu->addAction(action);
	}
}

/* }}} */

VOID_NAMESPACE_CLOSE