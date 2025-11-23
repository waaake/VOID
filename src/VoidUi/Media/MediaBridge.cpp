// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QCoreApplication>
#include <QDataStream>

/* Internal */
#include "MediaBridge.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Preferences/Preferences.h"

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
    , m_RecentProjectsMenu(nullptr)
{
    m_Projects = new ProjectModel(this);
    m_UndoGroup = new QUndoGroup(this);

    /* Setup a Default Project */
    NewProject();
    // DefaultProject();

    connect(&VoidPreferences::Instance(), &VoidPreferences::projectsUpdated, this, &MBridge::ResetProjectsMenu);
}

MBridge::~MBridge()
{
    m_Projects->deleteLater();
    delete m_Projects;
    m_Projects = nullptr;

    m_RecentProjectsMenu->deleteLater();
    delete m_RecentProjectsMenu;
    m_RecentProjectsMenu = nullptr;
}

void MBridge::DefaultProject()
{
    VOID_LOG_INFO("Setting Default Project...");

    std::string recent = VoidPreferences::Instance().MostRecentProject();

    VOID_LOG_INFO("Last Project Path: {0}", recent);

    NewProject();

    // if (!recent.empty())
    //     Load(recent);
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

void MBridge::RemoveMedia(const QModelIndex& index)
{
    PushCommand(new MediaRemoveCommand(index));
}

void MBridge::RemoveMedia(const std::vector<QModelIndex>& indexes)
{
    if (m_Project)
    {
        QUndoStack* stack = m_Project->UndoStack();
        stack->beginMacro("Remove Media");

        /**
         * Loop over in a a reverse way as forward iteration would shift the model indexes and
         * result in wrong indexes being deleted, or a worst case scenario result in crashes as
         * the second model index doesn't even exist after the first has been deleted
         */
        for (int i = indexes.size() - 1; i >= 0; --i)
        {
            QModelIndex idx = indexes.at(i);
            stack->push(new MediaRemoveCommand(idx));
        }

        stack->endMacro();
    }
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

void MBridge::AddToPlaylist(QByteArray& data)
{
    AddToPlaylist(data, ActivePlaylist());
}

void MBridge::AddToPlaylist(QByteArray& data, Playlist* playlist)
{
    if (m_Project)
    {
        QDataStream stream(&data, QIODevice::ReadOnly);
        int count;
        stream >> count;
    
        QUndoStack* stack = m_Project->UndoStack();
        stack->beginMacro("Add Media to Playlist");

        for (int i = 0; i < count; ++i)
        {
            int row, column;
            stream >> row >> column;

            stack->push(new PlaylistAddMediaCommand(m_Project->DataModel()->index(row, column), playlist));
        }

        stack->endMacro();
    }
}

void MBridge::RemoveFromPlaylist(const QModelIndex& index)
{
    PushCommand(new PlaylistRemoveMediaCommand(index));
}

void MBridge::RemoveFromPlaylist(const std::vector<QModelIndex>& indexes)
{
    if (m_Project)
    {
        QUndoStack* stack = m_Project->UndoStack();
        stack->beginMacro("Remove Media from Playlist");

        for (const QModelIndex& index : indexes)
            stack->push(new PlaylistRemoveMediaCommand(index));

        stack->endMacro();
    }
}

void MBridge::RemoveFromPlaylist(const QModelIndex& index, Playlist* playlist)
{
    PushCommand(new PlaylistRemoveMediaCommand(index, playlist));
}

void MBridge::RemoveFromPlaylist(const std::vector<QModelIndex>& indexes, Playlist* playlist)
{
    if (m_Project)
    {
        QUndoStack* stack = m_Project->UndoStack();
        stack->beginMacro("Remove Media from Playlist");

        for (const QModelIndex& index : indexes)
            stack->push(new PlaylistRemoveMediaCommand(index, playlist));

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
    {
        m_Project->UndoStack()->push(new PlaylistAddCommand());
        return m_Project->ActivePlaylist();
    }

    return nullptr;
}

Playlist* MBridge::NewPlaylist(const std::string& name)
{
    if (m_Project)
    {
        m_Project->UndoStack()->push(new PlaylistAddCommand(name));
        return m_Project->ActivePlaylist();
    }

    return nullptr;
}

void MBridge::RemovePlaylist(const QModelIndex& index)
{
    PushCommand(new PlaylistRemoveCommand(index));
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

        /* Add the current project path to Recents */
        VoidPreferences::Instance().AddRecentProject(path);
        return true;
    }

    return false;
}

QByteArray MBridge::PackIndexes(const std::vector<QModelIndex>& indexes) const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    /* Save the count of the Media first -- followed by media indices */
    stream << static_cast<int>(indexes.size());
    for (const QModelIndex& index : indexes)
        stream << index.row() << index.column();

    return data;
}

std::vector<SharedMediaClip> MBridge::UnpackProjectMedia(QByteArray& data) const
{
    QDataStream stream(&data, QIODevice::ReadOnly);
    int count;
    stream >> count;

    std::vector<SharedMediaClip> clips;
    clips.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        int row, column;
        stream >> row >> column;

        clips.emplace_back(m_Project->MediaAt(row, column));
    }

    return clips;
}

std::vector<SharedMediaClip> MBridge::UnpackPlaylistMedia(QByteArray& data) const
{
    QDataStream stream(&data, QIODevice::ReadOnly);
    int count;
    stream >> count;

    std::vector<SharedMediaClip> clips;
    clips.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        int row, column;
        stream >> row >> column;

        clips.emplace_back(m_Project->ActivePlaylist()->Media(row, column));
    }

    return clips;
}

SharedPlaybackTrack MBridge::AsTrack(const std::vector<SharedMediaClip>& media) const
{
    SharedPlaybackTrack track = std::make_shared<PlaybackTrack>();

    for (const SharedMediaClip& clip : media)
        track->AddMedia(clip);

    return track;
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

    /* Save as the Last project Opened */
    VoidPreferences::Instance().AddRecentProject(path);

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
    emit updated();
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

    emit updated();

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

    emit updated();

    return true;
}

QMenu* MBridge::RecentProjectsMenu(QMenu* parent)
{
    if (!m_RecentProjectsMenu)
        m_RecentProjectsMenu = new QMenu("Recent Projects", parent);
    
    ResetProjectsMenu();
    return m_RecentProjectsMenu;
}

void MBridge::ResetProjectsMenu()
{
    m_RecentProjectsMenu->clear();

    for (const std::string& project : VoidPreferences::Instance().RecentProjects())
    {
        QAction* action = new QAction(project.c_str(), m_RecentProjectsMenu);
        connect(action, &QAction::triggered, this, [=]() { Load(project); });

        m_RecentProjectsMenu->addAction(action);
    }
}

VOID_NAMESPACE_CLOSE
