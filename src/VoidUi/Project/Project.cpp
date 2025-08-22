// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Project.h"

VOID_NAMESPACE_OPEN

Project::Project(const std::string& name, bool active, QObject* parent)
    : Core::Project(name, active, parent)
{
    m_UndoStack = new QUndoStack(this);
}

Project::Project(bool active, QObject* parent)
    : Core::Project(active, parent)
{
}

Project::~Project()
{
    m_UndoStack->deleteLater();
}

VOID_NAMESPACE_CLOSE
