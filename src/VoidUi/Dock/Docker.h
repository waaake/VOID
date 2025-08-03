// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_DOCKER_H
#define _VOID_DOCKER_H

/* Qt */
#include <QDockWidget>
#include <QMenu>
#include <QSplitter>
#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>

/* Internal */
#include "Definition.h"
#include "DockPanel.h"
#include "DockSplitter.h"

VOID_NAMESPACE_OPEN

class VoidDocker : public QDockWidget
{
    Q_OBJECT

public:
    VoidDocker(QWidget* parent = nullptr);
    VoidDocker(const std::string& title, QWidget*parent = nullptr);
    virtual ~VoidDocker();
    
    /* Helpers */

    /*
     * Adjusts the Dock panel's closable property
     * If true, the dock panel gets a close button and can be closed from it
     * false would get rid of the close button
     */
    void SetClosable(const bool closable);

};

class DockTab : public QTabBar
{
	Q_OBJECT

public:
	DockTab(QWidget* parent = nullptr);

protected:
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

signals:
	void tabDetachRequested(int index, const QPoint& position);

private: /* Members */
	QPoint m_StartPos;
	bool m_Dragging;
};

class DockWidget : public QTabWidget
{
	Q_OBJECT

public:
	DockWidget(DockSplitter* parent = nullptr);

	void AddDock(QWidget* panel, const std::string& title, bool closable = false);
	void AddDockManagerWidget(int index);

signals:
	/* Emitted when the DockWidget is to be closed */
	void closureRequested(QWidget*);
	void splitRequested(const Qt::Orientation&);

private: /* Members */
	DockTab* m_DockTab;
	QToolButton* m_PanelOptions;

	QMenu* m_Options;
	QMenu* m_DockMenu;
	QAction* m_ClosePaneAction;
	QAction* m_SplitHorizontalAction;
	QAction* m_SplitVerticalAction;

	DockSplitter* m_Splitter;

private: /* Methods */
	void CloseTab(int index);
	void UndockTab(int index, const QPoint& positiion);
	void SetTabClosable(int index);

	/**
	 * Sets up the Dock Panel Options Tool button
	 */
	void SetupOptions();

	void Connect();

	/* Closes the entire Pane of tab widgets */
	void ClosePane();

	/* (Re)setup the Menu for the Docks */
	void ResetDockMenu();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCKER_H
