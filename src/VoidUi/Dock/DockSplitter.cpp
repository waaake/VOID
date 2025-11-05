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

int DockSplitter::AddPane()
{
	DockWidget* docker = new DockWidget(this);
	return AddWidget(docker);
}

int DockSplitter::AddPane(int id)
{
	DockWidget* docker = new DockWidget(this);
	/* Add the Widget from the DockManager to the dock */
	docker->AddDockManagerWidget(id);
	return AddWidget(docker);
}

int DockSplitter::AddPanes(const std::vector<int>& ids)
{
	DockWidget* docker = new DockWidget(this);

	for (int id : ids)
		docker->AddDockManagerWidget(id);

	return AddWidget(docker);
}

int DockSplitter::AddPane(QWidget* widget, const std::string& name, bool closable)
{
	DockWidget* docker = new DockWidget(this);
	/* Add the Widget to the dock */
	docker->AddDock(widget, name, closable);
	return AddWidget(docker);
}

int DockSplitter::AddSplitPane(int idA, int idB, const Qt::Orientation& orientation)
{
	DockSplitter* splitter = new DockSplitter(orientation, this);
	splitter->AddPane(idA);
	splitter->AddPane(idB);

	int index = count() - 1;
	addWidget(splitter);

	return index;
}

void DockSplitter::RemovePane(int index)
{
	VOID_LOG_INFO("INDEX: {0}", index);

	QWidget* w = findChild<QWidget*>(QString::number(index));

	/* Set parent of internal children as null as to not delete them when the Pane is deleted/removed */
	for (QWidget*& c : w->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
		c->setParent(nullptr);

	/* Remove Widget */
	w->hide();
	w->deleteLater();

	/* Reset the sizes */
	setSizes({1});
}

void DockSplitter::ClearPanes()
{
	/* Get All of Constructed DockPanes */
	for (DockWidget*& w : findChildren<DockWidget*>(QString()))
	{
		/* Set parent of internal children as null as to not delete them when the Pane is deleted/removed */
		for (QWidget*& c : w->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
			c->setParent(nullptr);

		w->hide();
		w->deleteLater();
	}
}

DockWidget* DockSplitter::Resplit(int index, const Qt::Orientation& orientation)
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
	return splitter->DockerAt(splitter->AddPane());
}

int DockSplitter::AddWidget(DockWidget* widget)
{
	/* This is the index on which the next widget will get added to */
	int index = count() - 1;
	widget->setObjectName(QString::number(index));

	/* Connect for removal */
	connect(widget, &DockWidget::closureRequested, this, [=]() { RemovePane(index); });
	/* Connect for split */
	connect(widget, &DockWidget::splitRequested, this, [=](const Qt::Orientation& orientation) { Resplit(index, orientation); });

	/* Add the docker to the pane */
	addWidget(widget);

	return index;
}

DockWidget* DockSplitter::DockerAt(int index)
{
	return dynamic_cast<DockWidget*>(widget(index));
}

VOID_NAMESPACE_CLOSE
