// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Internal */
#include "PlaylistCommands.h"

VOID_NAMESPACE_OPEN

/// PlaylistAddCommand

PlaylistAddCommand::PlaylistAddCommand(Project* project, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Project(project)
{
    m_InsertIndex = project->PlaylistMediaModel()->rowCount();
    setText("Add Playlist");
}

PlaylistAddCommand::PlaylistAddCommand(Project* project, const std::string& name, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Name(name)
    , m_Project(project)
{
    m_InsertIndex = project->PlaylistMediaModel()->rowCount();
    setText("Add Playlist");
}

void PlaylistAddCommand::undo()
{
    QModelIndex index = m_Project->PlaylistMediaModel()->index(m_InsertIndex, 0);
    m_Project->RemovePlaylist(index);
}

bool PlaylistAddCommand::Redo()
{
    if (Playlist* playlist = m_Name.empty() ? m_Project->NewPlaylist() : m_Project->NewPlaylist(m_Name))
    {
        // Update name to be created back again with this name
        m_Name = playlist->Name();
        return true;
    }

    return false;
}

/// PlaylistRemoveCommand

PlaylistRemoveCommand::PlaylistRemoveCommand(Project* project, const QModelIndex& index, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Project(project)
{
    setText("Remove Playlist");
}

void PlaylistRemoveCommand::undo()
{
    Playlist* playlist = new Playlist("playlist", m_Project);
    std::istringstream is(m_Data, std::ios::binary);
    playlist->Deserialize(is);
    m_Project->InsertPlaylist(playlist, m_Index.row());
}

bool PlaylistRemoveCommand::Redo()
{
    if (Playlist* playlist = m_Project->PlaylistAt(m_Index))
    {
        std::ostringstream os(std::ios::binary);
        playlist->Serialize(os);
        m_Data = os.str();

        m_Project->RemovePlaylist(m_Index);
        return true;
    }

    return false;
}

/// PlaylistAddMediaCommand

PlaylistAddMediaCommand::PlaylistAddMediaCommand(Core::Project* project, const QModelIndex& index, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Project(project)
{
    Playlist* playlist = project->ActivePlaylist();
    m_PlaylistIndex = project->PlaylistMediaModel()->PlaylistRow(playlist);
    m_InsertIndex = playlist->DataModel()->rowCount();

    setText("Add Media to Playlist");
}

PlaylistAddMediaCommand::PlaylistAddMediaCommand(const QModelIndex& index, Playlist* playlist, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Project(playlist->Project())
{
    m_PlaylistIndex = m_Project->PlaylistMediaModel()->PlaylistRow(playlist);
    m_InsertIndex = playlist->DataModel()->rowCount();
    setText("Add Media To Playlist");
}

void PlaylistAddMediaCommand::undo()
{
    if (Playlist* playlist = m_Project->PlaylistAt(m_PlaylistIndex, 0))
    {
        QModelIndex index = playlist->DataModel()->index(m_InsertIndex, 0);
        playlist->RemoveMedia(index);
    }
}

bool PlaylistAddMediaCommand::Redo()
{
    if (Playlist* playlist = m_Project->PlaylistAt(m_PlaylistIndex, 0))
        return playlist->AddMedia(m_Project->MediaAt(m_Index));

    return false;
}

/// Playlist Remove Media Command

PlaylistRemoveMediaCommand::PlaylistRemoveMediaCommand(Core::Project* project, const QModelIndex& index, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Project(project)
{
    Playlist* playlist = project->ActivePlaylist();
    m_PlaylistIndex = project->PlaylistMediaModel()->PlaylistRow(playlist);

    setText("Remove Media from Playlist");
}

PlaylistRemoveMediaCommand::PlaylistRemoveMediaCommand(const QModelIndex& index, Playlist* playlist, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Project(playlist->Project())
{
    m_PlaylistIndex = m_Project->PlaylistMediaModel()->PlaylistRow(playlist);
    setText("Remove Media from Playlist");
}

void PlaylistRemoveMediaCommand::undo()
{
    if (Playlist* playlist = m_Project->PlaylistAt(m_PlaylistIndex, 0))
        playlist->InsertMedia(m_Project->MediaAt(m_MediaIndex), m_Index.row());
}

bool PlaylistRemoveMediaCommand::Redo()
{
    if (Playlist* playlist = m_Project->PlaylistAt(m_PlaylistIndex, 0))
    {
        /**
         * Get the actual index of the Media in the project
         * Since the media in the playlist is just referencing the Media from the project it belongs to,
         * this index would be then used to re insert this back when the command is undone
         */
        const SharedMediaClip& clip = playlist->Media(m_Index);
        const EntityModel* model = m_Project->DataModel();
        m_MediaIndex = model->index(model->MediaRow(clip), 0);

        return playlist->RemoveMedia(m_Index);
    }

    return false;
}

/* }}} */

VOID_NAMESPACE_CLOSE
