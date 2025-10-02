// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QCoreApplication>

/* Internal */
#include "MediaBridge.h"
#include "VoidCore/Logging.h"

/* Commands */
#include "VoidUi/Commands/MediaCommands.h"
#include "VoidUi/Commands/PlaylistCommands.h"

VOID_NAMESPACE_OPEN

MBridge& MBridge::Instance()
{
    static MBridge instance;
    return instance;
}

MBridge::MBridge(QObject* parent)
    : QObject(parent)
{
    m_Projects = new ProjectModel(this);
    m_UndoGroup = new QUndoGroup(this);

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
    /* Connect Project Signals */
    connect(m_Project, &Project::playlistCreated, this, &MBridge::playlistCreated);
    connect(m_Project, &Project::playlistChanged, this, &MBridge::playlistChanged);
}

void MBridge::SetCurrentProject(int index)
{
    /* Provided index is not valid */
    if (index > m_Projects->rowCount() - 1)
        return;

    SetActiveProject(dynamic_cast<Project*>(m_Projects->ProjectAt(m_Projects->index(index, 0))));
    emit projectChanged(m_Project);
}

void MBridge::SetCurrentProject(const QModelIndex& index)
{
    /* Provided index is not valid */
    if (!index.isValid())
        return;

    SetActiveProject(dynamic_cast<Project*>(m_Projects->ProjectAt(index)));
    emit projectChanged(m_Project);
}

void MBridge::SetCurrentProject(Project* project)
{
    SetActiveProject(project);
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

    m_UndoGroup->addStack(m_Project->UndoStack());
    m_UndoGroup->setActiveStack(m_Project->UndoStack());

    /* Force Update on the Model */
    m_Projects->Refresh();
}

void MBridge::AddMedia(const std::string& filepath)
{
    PushCommand(new MediaImportCommand(filepath));
}

void MBridge::RemoveMedia(const std::vector<QModelIndex>& media)
{
    PushCommand(new MediaRemoveCommand(media));
}

void MBridge::AddToPlaylist(const QModelIndex& index)
{
    PushCommand(new PlaylistAddMediaCommand(index));
}

void MBridge::AddToPlaylist(const std::vector<QModelIndex>& indexes)
{
    if (m_Project)
    {
        QUndoStack* stack = m_Project->UndoStack();
        stack->beginMacro("Add Media to Playlist");

        for (const QModelIndex& index : indexes)
            stack->push(new PlaylistAddMediaCommand(index));

        stack->endMacro();
    }
}

void MBridge::AddToPlaylist(const QModelIndex& index, Playlist* playlist)
{
    PushCommand(new PlaylistAddMediaCommand(index, playlist));
}

void MBridge::AddToPlaylist(const std::vector<QModelIndex>& indexes, Playlist* playlist)
{
    if (m_Project)
    {
        QUndoStack* stack = m_Project->UndoStack();
        stack->beginMacro("Add Media to Playlist");

        for (const QModelIndex& index : indexes)
            stack->push(new PlaylistAddMediaCommand(index, playlist));

        stack->endMacro();
    }
}

bool MBridge::AddMedia(const MediaStruct& mstruct)
{
    /* Create the Media Clip */
    SharedMediaClip clip = std::make_shared<MediaClip>(mstruct, this);

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
    SharedMediaClip clip = std::make_shared<MediaClip>(mstruct, this);

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
    QCoreApplication::processEvents();

    /* Remove this from the Underlying model */
    m_Project->RemoveMedia(m_Project->ClipIndex(clip));

    return true;
}

bool MBridge::Remove(const QModelIndex& index)
{
    /* The Media Associated with the Model index */
    SharedMediaClip clip = m_Project->MediaAt(index);

    /* Invalid Index */
    if (!clip)
        return false;

    /* Emit the mediaAboutToBeRemoved signal for all listeners to clear the item */
    emit mediaAboutToBeRemoved(clip);

    /* Ensure All events are Processed before deleting the Media Clip internally */
    QCoreApplication::processEvents();

    /* Remove this from the Underlying model */
    m_Project->RemoveMedia(index);

    return true;
}

Playlist* MBridge::NewPlaylist()
{
    if (m_Project)
        return m_Project->NewPlaylist();

    return nullptr;
}

Playlist* MBridge::NewPlaylist(const std::string& name)
{
    if (m_Project)
        return m_Project->NewPlaylist(name);

    return nullptr;
}

void MBridge::SetCurrentPlaylist(const QModelIndex& index)
{
    if (m_Project)
        m_Project->SetCurrentPlaylist(index);
}

void MBridge::SetCurrentPlaylist(int row)
{
    if (m_Project)
        m_Project->SetCurrentPlaylist(row);
}

void MBridge::PushCommand(QUndoCommand* command)
{
    /* Push it on the Undo Stack of the active project */
    if (m_Project)
        m_Project->PushCommand(command);
}

bool MBridge::Save()
{
    if (!m_Project)
        return false;

    if (m_Project->Save())
    {
        /* Force Update on the Model */
        m_Projects->Refresh();
        return true;
    }

    return false;
}

bool MBridge::Save(const std::string& path, const std::string& name, const EtherFormat::Type& type)
{
    if (!m_Project)
        return false;

    if (m_Project->Save(path, name, type))
    {
        /* Force Update on the Model */
        m_Projects->Refresh();
        return true;
    }

    return false;
}

void MBridge::Load(const std::string& path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open())
    {
        VOID_LOG_ERROR("Failed to Open File {0}", path);
        return;
    }

    /* Read the File header to understand whether this is a valid file */
    char header[EtherFormat::MAGIC_SIZE] = {};
    in.read(header, EtherFormat::MAGIC_SIZE);

    EtherFormat::Type type = EtherFormat::FileType(header);
    if (type == EtherFormat::Type::INVALID)
    {
        VOID_LOG_INFO("Invalid File format");
        return;
    }

    if (type == EtherFormat::Type::ASCII)
    {
        std::stringstream buffer;
        buffer << in.rdbuf();
        SetActiveProject(Project::FromDocument(buffer.str()));
    }
    else
    {
        SetActiveProject(Project::FromStream(in));
    }

    connect(m_Project, &Project::playlistCreated, this, &MBridge::playlistCreated);
    connect(m_Project, &Project::playlistChanged, this, &MBridge::playlistChanged);

    /* Add to the projects */
    m_Projects->Add(m_Project);
    m_Project->SetSavePath(path);

    emit projectCreated(m_Project);
}

bool MBridge::CloseProject(bool force)
{
    if (!m_Project)
        return true; // No project to close

    /* Project needs saving if not forced*/
    if (m_Project->Modified() && !force)
        return false;

    /* Case where the current project is the last one */
    bool create = m_Projects->rowCount() == 1;

    int row = m_Projects->ProjectRow(m_Project);
    QModelIndex index = m_Projects->index(row, 0);

    /* Remove the targeted project */
    m_Projects->Remove(index);

    if (create)
        NewProject();
    else
        SetCurrentProject(m_Projects->index(row == 0 ? ++row : --row, 0));

    return true;
}

bool MBridge::CloseProject(Project* project, bool force)
{
    /* It's the current project */
    if (project == m_Project)
        return CloseProject(force);

    /* Project needs saving if not forced*/
    if (m_Project->Modified() && !force)
        return false;

    /* Case where the current project is the last one */
    bool create = m_Projects->rowCount() == 1;

    int row = m_Projects->ProjectRow(project);
    QModelIndex index = m_Projects->index(row, 0);

    /* Remove the targeted project */
    m_Projects->Remove(index);

    if (create)
        NewProject();

    return true;
}

VOID_NAMESPACE_CLOSE
