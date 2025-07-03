#ifndef _VOID_DOCK_WIDGET_H
#define _VOID_DOCK_WIDGET_H

/* Qt */
#include <QTabWidget>
#include <QTabBar>

/* Internal */
#include "Definition.h"
#include "DockPanel.h"

VOID_NAMESPACE_OPEN

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
	DockWidget(QWidget* parent = nullptr);

	void AddDock(QWidget* panel, const std::string& title, bool closable = false);

private: /* Members */
	DockTab* m_DockTab;

private: /* Methods */
	void CloseTab(int index);
	void UndockTab(int index, const QPoint& positiion);
	void SetTabClosable(int index);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCK_WIDGET_H