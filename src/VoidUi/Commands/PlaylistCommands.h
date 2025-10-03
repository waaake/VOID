// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYLIST_COMMANDS_H
#define _VOID_PLAYLIST_COMMANDS_H

/* STD */
#include <map>
#include <vector>

/* Internal */
#include "Definition.h"
#include "VoidCommand.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

class PlaylistAddMediaCommand : public VoidUndoCommand
{
public:
    PlaylistAddMediaCommand(const QModelIndex& index, QUndoCommand* parent = nullptr);
    PlaylistAddMediaCommand(const QModelIndex& index, Playlist* playlist, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private: /* Members */
    QModelIndex m_Index;
    Playlist* m_Playlist;
    unsigned int m_InsertIndex;
};

class PlaylistRemoveMediaCommand : public VoidUndoCommand
{
public:
    PlaylistRemoveMediaCommand(const QModelIndex& index, QUndoCommand* parent = nullptr);
    PlaylistRemoveMediaCommand(const QModelIndex& index, Playlist* playlist, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    QModelIndex m_Index;
    QModelIndex m_MediaIndex;
    Playlist* m_Playlist;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYLIST_COMMANDS_H
