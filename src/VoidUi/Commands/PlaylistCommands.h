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

class PlaylistAddCommand : public VoidUndoCommand
{
public:
    explicit PlaylistAddCommand(QUndoCommand* parent = nullptr);
    PlaylistAddCommand(const std::string& name, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    unsigned int m_InsertIndex;
    std::string m_Name;
};

class PlaylistRemoveCommand : public VoidUndoCommand
{
public:
    PlaylistRemoveCommand(const QModelIndex& index, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    QModelIndex m_Index;
    std::string m_Name;

    /**
     * A playlist may or may not contain media added from the project
     * in any case, we need to store the media indexes which it references from the project
     * so when undone, the media gets referenced back into the playlist
     * 
     * TODO: Check if there is an optimal solution to querying multiple media indexes and also 
     * a better way to add them back to the playlist as soon as it's created
     * Should we look at serializing the entire playlist and recreate it back? or getting the indexes
     * and adding them back is faster?
     */
    std::vector<QModelIndex> m_MediaIndexes;
};

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
