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

/* Internal */
#include "QDefinition.h"
#include "DockPanel.h"
#include "DockSplitter.h"
#include "VoidQExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidDocker : public QDockWidget
{
public:
    VoidDocker(QWidget* parent = nullptr);
    VoidDocker(const std::string& title, QWidget*parent = nullptr);
    virtual ~VoidDocker();

    /*
     * Adjusts the Dock panel's closable property
     * If true, the dock panel gets a close button and can be closed from it
     * false would get rid of the close button
     */
    void SetClosable(const bool closable);

};

class VOID_API DockTab : public QTabBar
{
	Q_OBJECT
public:
	DockTab(QWidget* parent = nullptr);

protected:
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

signals:
	void tabDragged(int index);
	void tabRemovalRequested(int index);
	void tabDetachRequested(int index, const QPoint& position);

private: /* Members */
	QPoint m_StartPos;
	bool m_Dragging;
};

class VOID_API DockWidget : public QTabWidget
{
	Q_OBJECT
public:
	DockWidget(DockSplitter* parent = nullptr, bool floating = false);

	DockPanel* AddDock(QWidget* panel, const std::string& title, bool closable = false);
	void AddDockManagerWidget(int index);

	void RemoveTab(int index);
	int DockTabIndex(const QString& name) const;

protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragLeaveEvent(QDragLeaveEvent* event) override;
	void dropEvent(QDropEvent* event) override;

	void paintEvent(QPaintEvent* event) override;

signals:
	void closureRequested(QWidget*);
	void splitRequested(const Qt::Orientation&);

private: /* Members */
	DockTab* m_DockTab;
	MenuToolButton* m_PanelOptions;

	QMenu* m_Options;
	QMenu* m_DockMenu;
	QAction* m_ClosePaneAction;
	QAction* m_SplitHorizontalAction;
	QAction* m_SplitVerticalAction;

	DockSplitter* m_Splitter;

	bool m_DragActive;
	bool m_Floating;

private: /* Methods */
	void HideTab(int index);
	void UndockTab(int index, const QPoint& positiion);
	void SetTabClosable(int index);
	void SetupOptions();
	void Connect();
	void ClosePane();
	void CloseParent();
	void ResetDockMenu();
	bool ShowIfDocked(const QString& name);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCKER_H
