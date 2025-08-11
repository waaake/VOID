// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Project.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

Project::Project(const std::string& name, bool active, QObject* parent)
    : VoidObject(parent)
    , m_Name(name)
    , m_Active(active)
{
    m_Media = new MediaModel(this);
    m_UndoStack = new QUndoStack(this);
    VOID_LOG_INFO("Project {0} Created: {1}", name, Vuid());
}

Project::Project(bool active, QObject* parent)
    : Project("Untitled", active, parent)
{
}

Project::~Project()
{
    m_UndoStack->deleteLater();

    m_Media->deleteLater();
    delete m_Media;
    m_Media = nullptr;
}

VOID_NAMESPACE_CLOSE
