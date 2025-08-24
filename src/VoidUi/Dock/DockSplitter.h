// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_DOCK_SPLITTER_H
#define _VOID_DOCK_SPLITTER_H

/* Qt */
#include <QSplitter>
#include <QWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/* Forward Declrations for the DockSplitter to use DockWidget */
class DockWidget;

class DockSplitter : public QSplitter
{
public:
    DockSplitter(Qt::Orientation orientation, QWidget* parent = nullptr);

    /* Add an Empty Pane */
	void AddPane();

	/**
	 * Add Pane from the given index referring to an entity from the DockManager
	 */
	void AddPane(int id);

    /* Adds the Widget to the Pane */
	void AddPane(QWidget* widget, const std::string& name, bool closable = false);

    /* Remove the pane at the given index */
	void RemovePane(int index);

    /**
     * Splits the Internal Widget at the given index
     * - Unparent's the widget
     * - Adds a nested Splitter at that index
     * - Adds the previous widget as a child of the split result
     * - Adds an empty pane to the other side of the Split
     */
	void Resplit(int index, const Qt::Orientation& orientation);

private: /* Methods */
    void AddWidget(DockWidget* widget);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCK_SPLITTER_H
