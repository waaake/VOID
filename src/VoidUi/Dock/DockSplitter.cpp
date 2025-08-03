// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Docker.h"
#include "DockSplitter.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

DockSplitter::DockSplitter(Qt::Orientation orientation, QWidget* parent)
    : QSplitter(orientation, parent)
{
    /* Splitter Handle to drag */
    setHandleWidth(2);
}

void DockSplitter::AddPane()
{
	addWidget(new DockWidget(this));
}

void DockSplitter::AddPane(int id)
{
	DockWidget* docker = new DockWidget(this);
	/* Add the Widget from the DockManager to the dock */
	docker->AddDockManagerWidget(id);

	/* This is the index on which the next widget will get added to */
	int index = count() - 1;

	/* Connect for removal */
	connect(docker, &DockWidget::closureRequested, this, [=]() { RemovePane(index); });
	/* Connect for split */
	connect(docker, &DockWidget::splitRequested, this, [=](const Qt::Orientation& orientation) { Resplit(index, orientation); });

	/* Add the docker to the pane */
	addWidget(docker);
}

void DockSplitter::AddPane(QWidget* widget, const std::string& name, bool closable)
{
	DockWidget* docker = new DockWidget(this);
	/* Add the Widget to the dock */
	docker->AddDock(widget, name, closable);

	/* This is the index on which the next widget will get added to */
	int index = count() - 1;

	/* Connect for removal */
	connect(docker, &DockWidget::closureRequested, this, [=]() { RemovePane(index); });

	/* Add the docker to the pane */
	addWidget(docker);
}

void DockSplitter::RemovePane(int index)
{
	VOID_LOG_INFO("INDEX: {0}", index);

	QWidget* w = widget(index);
	/* Remove Widget */
	w->hide();
	w->deleteLater();

	/* Reset the sizes */
	setSizes({1});
}

void DockSplitter::Resplit(int index, const Qt::Orientation& orientation)
{
	/* Get the Existing Widget at the index */
	QWidget* w = widget(index);

	/* Unparent as to not delete it */
	w->setParent(nullptr);

	/* Now Insert the Splitter here */
	DockSplitter* splitter = new DockSplitter(orientation, this);
	insertWidget(index, splitter);

    // /* Preserve the when splitting vertically */
    // int width = w->width();

	/* Add the Existing Widget back to the splitter */
	splitter->addWidget(w);

	/* And Another Pane to the splitter */
	splitter->AddPane();
}

VOID_NAMESPACE_CLOSE
