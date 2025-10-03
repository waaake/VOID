// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "PlaylistCommands.h"

VOID_NAMESPACE_OPEN

/* Playlist Add Media Command {{{ */

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

/* }}} */

/* Playlist Remove Media Command {{{ */

PlaylistRemoveMediaCommand::PlaylistRemoveMediaCommand(const QModelIndex& index, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
{
    m_Playlist = _MediaBridge.ActivePlaylist();

    setText("Remove Media from Playlist");
}

PlaylistRemoveMediaCommand::PlaylistRemoveMediaCommand(const QModelIndex& index, Playlist* playlist, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Playlist(playlist)
{
    setText("Remove Media from Playlist");
}

void PlaylistRemoveMediaCommand::undo()
{
    if (m_Playlist)
        m_Playlist->InsertMedia(_MediaBridge.MediaAt(m_MediaIndex), m_Index.row());
}

bool PlaylistRemoveMediaCommand::Redo()
{
    if (m_Playlist)
    {
        /**
         * Get the actual index of the Media in the project
         * Since the media in the playlist is just referencing the Media from the project it belongs to,
         * this index would be then used to re insert this back when the command is undone
         */
        const SharedMediaClip& clip = m_Playlist->Media(m_Index);
        const MediaModel* model = _MediaBridge.DataModel();
        m_MediaIndex = model->index(model->MediaRow(clip), 0);

        return m_Playlist->RemoveMedia(m_Index);
    }

    return false;
}

/* }}} */

VOID_NAMESPACE_CLOSE
