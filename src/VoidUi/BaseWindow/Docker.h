#ifndef _VOID_DOCKER_H
#define _VOID_DOCKER_H

/* Qt */
#include <QDockWidget>
#include <QSplitter>
#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>

/* Internal */
#include "Definition.h"
#include "DockPanel.h"

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


class DockSplitter : public QSplitter
{
public:
    DockSplitter(Qt::Orientation orientation, QWidget* parent = nullptr);
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
	DockWidget(QWidget* parent = nullptr);

	void AddDock(QWidget* panel, const std::string& title, bool closable = false);

private: /* Members */
	DockTab* m_DockTab;
	QToolButton* m_PanelOptions;

private: /* Methods */
	void CloseTab(int index);
	void UndockTab(int index, const QPoint& positiion);
	void SetTabClosable(int index);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCKER_H
