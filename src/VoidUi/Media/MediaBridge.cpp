// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QApplication>

/* Internal */
#include "MediaBridge.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

MBridge::MBridge(QObject* parent)
    : QObject(parent)
{
    m_Projects = new ProjectModel(this);
    m_UndoStack = new QUndoStack(this);

    /* Setup a Default Project */
    NewProject();
}

MBridge::~MBridge()
{
    m_Projects->deleteLater();
    delete m_Projects;
    m_Projects = nullptr;
}

void MBridge::NewProject()
{
    /* Use default name */
    std::string name = "Project ";
    name += std::to_string(m_Projects->rowCount() + 1);

    NewProject(name);
}

void MBridge::NewProject(const std::string& name)
{
    SetActiveProject(new Project(name, true, this));

    /* Add to the projects */
    m_Projects->Add(m_Project);
    emit projectCreated(m_Project);
}

void MBridge::SetCurrentProject(int index)
{
    /* Provided index is not valid */
    if (index > m_Projects->rowCount() - 1)
        return;

    SetActiveProject(m_Projects->GetProject(m_Projects->index(index, 0)));
    emit projectChanged(m_Project);
}

void MBridge::SetCurrentProject(const QModelIndex& index)
{
    /* Provided index is not valid */
    if (!index.isValid())
        return;

    SetActiveProject(m_Projects->GetProject(index));
    emit projectChanged(m_Project);
}

void MBridge::SetActiveProject(Project* project)
{
    if (!project)
        return;

    /* Mark the current one as inactive */
    if (m_Project)
        m_Project->SetActive(false);

    m_Project = project;
    m_Project->SetActive(true);

    /* Force Update on the Model */
    m_Projects->Refresh();
}

bool MBridge::AddMedia(const MediaStruct& mstruct)
{
    /* Create the Media Clip */
    SharedMediaClip clip = std::make_shared<MediaClip>(Media(mstruct), this);

    /* Check if the clip is valid, there could be cases we don't have a specific media reader */
    if (clip->Empty())
    {
        VOID_LOG_INFO("Invalid Media.");
        return false;
    }

    /* Add to the underlying struct */
    m_Project->AddMedia(clip);

    /* Emit that we have added a new media clip now */
    emit mediaAdded(clip);

    /* Added successfully */
    return true;
}

bool MBridge::InsertMedia(const MediaStruct& mstruct, const int index)
{
    /* Create the Media Clip */
    SharedMediaClip clip = std::make_shared<MediaClip>(Media(mstruct), this);

    /* Check if the clip is valid, there could be cases we don't have a specific media reader */
    if (clip->Empty())
    {
        VOID_LOG_INFO("Invalid Media.");
        return false;
    }

    /* Add to the underlying struct */
    m_Project->InsertMedia(clip, index);

    /* Emit that we have added a new media clip now */
    emit mediaAdded(clip);

    /* Added successfully */
    return true;

}

bool MBridge::Remove(SharedMediaClip clip)
{
    /* Emit the mediaAboutToBeRemoved signal for all listeners to clear the item */
    emit mediaAboutToBeRemoved(clip);

    /* Ensure All events are Processed before deleting the Media Clip internally */
    QApplication::processEvents();

    /* Remove this from the Underlying model */
    m_Project->RemoveMedia(m_Project->ClipIndex(clip));

    return true;
}

bool MBridge::Remove(const QModelIndex& index)
{
    /* The Media Associated with the Model index */
    SharedMediaClip clip = m_Project->Media(index);

    /* Invalid Index */
    if (!clip)
        return false;

    /* Emit the mediaAboutToBeRemoved signal for all listeners to clear the item */
    emit mediaAboutToBeRemoved(clip);

    /* Ensure All events are Processed before deleting the Media Clip internally */
    QApplication::processEvents();

    /* Remove this from the Underlying model */
    m_Project->RemoveMedia(index);

    return true;
}

void MBridge::PushCommand(QUndoCommand* command)
{
    /* Push it on the Undo Stack */
    m_UndoStack->push(command);
}

VOID_NAMESPACE_CLOSE
