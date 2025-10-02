// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "PlaylistCommands.h"

VOID_NAMESPACE_OPEN

PlaylistAddMediaCommand::PlaylistAddMediaCommand(const QModelIndex& index, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
{
    m_Playlist = _MediaBridge.ActivePlaylist();
    m_InsertIndex = m_Playlist->DataModel()->rowCount();

    setText("Add Media to Playlist");
}

PlaylistAddMediaCommand::PlaylistAddMediaCommand(const QModelIndex& index, Playlist* playlist, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Playlist(playlist)
{
    m_InsertIndex = m_Playlist->DataModel()->rowCount();

    setText("Add Media To Playlist");
}

void PlaylistAddMediaCommand::undo()
{
    if (m_Playlist)
    {
        QModelIndex index = m_Playlist->DataModel()->index(m_InsertIndex, 0);
        m_Playlist->RemoveMedia(index);
    }
}

bool PlaylistAddMediaCommand::Redo()
{
    if (m_Playlist)
        return m_Playlist->AddMedia(_MediaBridge.MediaAt(m_Index));

    return false;
}

VOID_NAMESPACE_CLOSE
