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
	int AddPane();

	/**
	 * Add Pane from the given index referring to an entity from the DockManager
	 */
	int AddPane(int id);

    /**
     * Add Panes from the given indexes referring to the entity components from the DockManager
     */
    int AddPanes(const std::vector<int>& ids);

    /* Adds the Widget to the Pane */
	int AddPane(QWidget* widget, const std::string& name, bool closable = false);

    /**
     * Add a pre-spilt pane with specified orientation
     * - Creates a New pane
     * - Splits the pane in the mentioned orientation
     * - Adds Widget A and Widget B to the split
     * - Returns the ID of the Splitter
     */
    int AddSplitPane(int idA, int idB, const Qt::Orientation& orientation = Qt::Horizontal);

    /* Remove the pane at the given index */
	void RemovePane(int index);
    void ClearPanes();

    /**
     * Splits the Internal Widget at the given index
     * - Unparent's the widget
     * - Adds a nested Splitter at that index
     * - Adds the previous widget as a child of the split result
     * - Adds an empty pane to the other side of the Split
     */
	DockWidget* Resplit(int index, const Qt::Orientation& orientation);

    /**
     * Returns the DockWidget Pane at the provided index
     */
    DockWidget* DockerAt(int index);

private: /* Methods */
    int AddWidget(DockWidget* widget);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCK_SPLITTER_H
