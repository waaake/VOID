// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYLIST_COMMANDS_H
#define _VOID_PLAYLIST_COMMANDS_H

/* STD */
#include <map>
#include <vector>

/* Internal */
#include "Definition.h"
#include "VoidMediaPlayer/Project/Project.h"
#include "VoidUndo/VoidCommand.h"

VOID_NAMESPACE_OPEN

class PlaylistAddCommand : public VoidUndoCommand
{
public:
    explicit PlaylistAddCommand(Project* project, QUndoCommand* parent = nullptr);
    PlaylistAddCommand(Project* project, const std::string& name, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    std::string m_Name;
    unsigned int m_InsertIndex;
    Project* m_Project;
};

class PlaylistRemoveCommand : public VoidUndoCommand
{
public:
    PlaylistRemoveCommand(Project* project, const QModelIndex& index, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    std::string m_Data;
    QModelIndex m_Index;
    Project* m_Project;

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
    PlaylistAddMediaCommand(Core::Project* project, const QModelIndex& index, QUndoCommand* parent = nullptr);
    PlaylistAddMediaCommand(const QModelIndex& index, Playlist* playlist, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private: /* Members */
    QModelIndex m_Index;
    Core::Project* m_Project;
    int m_PlaylistIndex;
    unsigned int m_InsertIndex;
};

class PlaylistRemoveMediaCommand : public VoidUndoCommand
{
public:
    PlaylistRemoveMediaCommand(Core::Project* project, const QModelIndex& index, QUndoCommand* parent = nullptr);
    PlaylistRemoveMediaCommand(const QModelIndex& index, Playlist* playlist, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    QModelIndex m_Index;
    QModelIndex m_MediaIndex;
    Core::Project* m_Project;
    int m_PlaylistIndex;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYLIST_COMMANDS_H
