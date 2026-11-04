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
	QWidget* w = findChild<QWidget*>(QString::number(index));
	// Set parent of internal children as null as to not delete them when the Pane is deleted/removed
	for (QWidget*& c : w->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
		c->setParent(nullptr);

	w->hide();
	w->deleteLater();
	setSizes({1});
}

void DockSplitter::ClearPanes()
{
	for (DockWidget*& w : findChildren<DockWidget*>(QString()))
	{
		// Set parent of internal children as null as to not delete them when the Pane is deleted/removed
		for (QWidget*& c : w->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
			c->setParent(nullptr);

		w->hide();
		w->deleteLater();
	}
}

DockWidget* DockSplitter::Resplit(int index, const Qt::Orientation& orientation)
{
	QWidget* w = widget(index);
	w->setParent(nullptr);

	DockSplitter* splitter = new DockSplitter(orientation, this);
	insertWidget(index, splitter);

    // /* Preserve the when splitting vertically */
    // int width = w->width();

	// Add the Existing Widget back to the splitter
	splitter->addWidget(w);
	return splitter->DockerAt(splitter->AddPane());
}

int DockSplitter::AddWidget(DockWidget* widget)
{
	int index = count() - 1;
	widget->setObjectName(QString::number(index));

	connect(widget, &DockWidget::closureRequested, this, [=]() { RemovePane(index); });
	connect(widget, &DockWidget::splitRequested, this, [=](const Qt::Orientation& orientation) { Resplit(index, orientation); });

	addWidget(widget);
	return index;
}

DockWidget* DockSplitter::DockerAt(int index)
{
	return dynamic_cast<DockWidget*>(widget(index));
}

void DockSplitter::CloseExisting(const QString& name)
{
	for (const auto& docker : findChildren<DockWidget*>())
    {
        int id = docker->DockTabIndex(name);
        if (id > -1)
            return docker->RemoveTab(id);
    }
}

VOID_NAMESPACE_CLOSE
