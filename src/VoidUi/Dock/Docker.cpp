// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QApplication>
#include <QByteArray>
#include <QDrag>
#include <QMainWindow>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QToolButton>

/* Internal */
#include "Docker.h"
#include "DockManager.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Engine/IconForge.h"

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
			DockWidget* p = dynamic_cast<DockWidget*>(parentWidget());
			int index = tabAt(m_StartPos);
			DockPanel* w = dynamic_cast<DockPanel*>(p->widget(index));
			QPixmap pixmap = w->grab();

			QDrag* drag = new QDrag(this);
			QMimeData* mime = new QMimeData;
			mime->setData(MimeTypes::TabIndex, QByteArray::number(w->PanelId()));
			drag->setMimeData(mime);
			drag->setPixmap(pixmap);

			Qt::DropAction drop = drag->exec(Qt::MoveAction);

			if (drop == Qt::IgnoreAction)
				emit tabDetachRequested(index, mapToGlobal(event->pos()));
			else
				emit tabRemovalRequested(index);
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

DockWidget::DockWidget(DockSplitter* parent, bool floating)
	: QTabWidget(parent)
	, m_Splitter(parent)
	, m_DragActive(false)
	, m_Floating(floating)
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

void DockWidget::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat(MimeTypes::TabIndex))
	{
		event->acceptProposedAction();

		m_DragActive = true;
		update();
	}
}

void DockWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
	m_DragActive = false;
	update();
}

void DockWidget::dropEvent(QDropEvent* event)
{
	QByteArray data = event->mimeData()->data(MimeTypes::TabIndex);
	int index = data.toInt();

	AddDockManagerWidget(index);

	m_DragActive = false;
	update();
	event->accept();
}

void DockWidget::paintEvent(QPaintEvent* event)
{
	QTabWidget::paintEvent(event);

	if (m_DragActive)
	{
		QPainter p(this);
		QColor highlight = palette().color(QPalette::Highlight);

		if (count())
		{
			p.fillRect(
				QRect(0, 0, width(), tabBar()->height()),
				QColor(highlight.red(), highlight.green(), highlight.blue(), 127)
			);
		}
		else
		{
			p.fillRect(rect(), QColor(highlight.red(), highlight.green(), highlight.blue(), 127));
		}
	}
}

void DockWidget::SetTabClosable(int index)
{
	CloseButton* closeButton = new CloseButton();
    closeButton->setFixedSize(12, 12);

	// connect(closeButton, &QToolButton::clicked, this, [=]() { emit tabCloseRequested(index); });
	connect(closeButton, &QToolButton::clicked, this, [=]() { RemoveTab(index); });

	/* Set the tool button on the tabbar */
	tabBar()->setTabButton(index, QTabBar::RightSide, closeButton);
}

void DockWidget::RemoveTab(int index)
{
	/* Dock panel at the index */
	DockPanel* panel = dynamic_cast<DockPanel*>(widget(index));
	panel->setParent(nullptr);

	/**
	 * If the current Widget is a floating panel -> we're free to destroy it
	 * if we don't have any more tabs left
	 */
	if (!count() && m_Floating)
		CloseParent();
}

void DockWidget::UndockTab(int index, const QPoint& position)
{
	QMainWindow* window = new QMainWindow;
	DockSplitter* splitter = new DockSplitter(Qt::Horizontal, window);
	DockWidget* undocked = new DockWidget(splitter, true);
	window->setCentralWidget(undocked);

	/* Get the Dock Panel at the index */
	DockPanel* panel = static_cast<DockPanel*>(widget(index));

	/* Unparent the Panel */
	panel->setParent(nullptr);
	undocked->AddDockManagerWidget(panel->PanelId());

	/* Move the window to provided position */
	window->move(position);
	window->show();
}

void DockWidget::SetupOptions()
{
	setAcceptDrops(true);

	/* Options Button */
	m_PanelOptions = new MenuToolButton;
	m_PanelOptions->setIcon(IconForge::GetIcon(IconType::icon_browse, _DARK_COLOR(QPalette::Text, 140)));
	m_PanelOptions->setAutoRaise(true);
	m_PanelOptions->setPopupMode(QToolButton::InstantPopup);

	/* Add to the Tab Widget, just before the tabs start */
	setCornerWidget(m_PanelOptions, Qt::TopLeftCorner);
	/* Minimum Size ensures the corner widget remains visible even when no tabs are */
	cornerWidget(Qt::TopLeftCorner)->setMinimumSize(m_PanelOptions->sizeHint());

	/* Options Menu */
	m_Options = new QMenu(m_PanelOptions);
	m_DockMenu = new QMenu("Docks", m_PanelOptions);

	m_ClosePaneAction = new QAction("Close Pane", m_Options);
	m_SplitHorizontalAction = new QAction("Split Horizontally", m_Options);
	m_SplitVerticalAction = new QAction("Split Vertically", m_Options);

	m_Options->addAction(m_ClosePaneAction);
	m_Options->addAction(m_SplitHorizontalAction);
	m_Options->addAction(m_SplitVerticalAction);

	/**
	 * TODO: For floating panels splitting is disabled at the moment
	 * it is based on the current architecture that we are not tracking how many splits
	 * have been added to the parent dock splitter, we can't really allow floating panels
	 * to be split, as with 0 tabs remaining, the window is destroyed
	 * Once the tracking on the parent Splitter is added, this can then be allowed
	 * and Windows can then be allowed to have a full feature as the main window for docking
	 */
	m_SplitHorizontalAction->setEnabled(!m_Floating);
	m_SplitVerticalAction->setEnabled(!m_Floating);

	/* ------------------------ */
	m_Options->addSeparator();

	m_Options->addMenu(m_DockMenu);
	m_PanelOptions->setMenu(m_Options);

	/* Setup Dock Menu from available options */
	ResetDockMenu();
}

void DockWidget::Connect()
{
	/* Tab Bar */
	connect(m_DockTab, &DockTab::tabRemovalRequested, this, &DockWidget::RemoveTab);
	connect(m_DockTab, &DockTab::tabDetachRequested, this, &DockWidget::UndockTab);

	/* Panel Options */
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

void DockWidget::CloseParent()
{
	/**
	 * The floating panels are constructed with the main window -> Dock Splitter -> Dock Widget (this)
	 * so from the dock widget we start getting the parent -> parent, i.e. the main window
	 * and free the pointer to the window, clearing all of the children
	 */
	QMainWindow* window = dynamic_cast<QMainWindow*>(parentWidget()->window());

	if (window)
	{
		window->deleteLater();
		delete window;
		window = nullptr;
	}
}

void DockWidget::ResetDockMenu()
{
	/* Clear all of the Dock Menu */
	m_DockMenu->clear();

	/* From the Available docks in the Dock Manager, setup Menu Actions */
	for (const std::pair<int, DockStruct>& it: DockManager::Instance().AvailableDocks())
	{
		QAction* action = new QAction(it.second.name.c_str(), m_DockMenu);
		connect(action, &QAction::triggered, this, [=]() { AddDockManagerWidget(it.first); });

		m_DockMenu->addAction(action);
	}
}

/* }}} */

VOID_NAMESPACE_CLOSE