// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_COMMANDS_H
#define _VOID_MEDIA_COMMANDS_H

/* STD */
#include <map>
#include <vector>

/* Internal */
#include "Definition.h"
#include "VoidCommand.h"
#include "VoidObjects/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

class MediaImportCommand : public VoidUndoCommand
{
public:
    MediaImportCommand(const std::string& path, QUndoCommand* parent = nullptr);

    /* Define the Undo Action */
    void undo() override;
    /* The return status governs if the command needs to be marked as obsolete when failed */
    bool Redo() override;

private: /* Members */
    size_t m_InsertIndex;
    /* Non Destructable entity that can construct the media */
    std::string m_Path;
};

class MediaRemoveCommand : public VoidUndoCommand
{
public:
    MediaRemoveCommand(const std::vector<QModelIndex>& indexes, QUndoCommand* parent = nullptr);

    /* Define the Undo Action */
    void undo() override;
    /* The return status governs if the command needs to be marked as obsolete when failed */
    bool Redo() override;

private: /* Members */
    /* Mapped to the index of the Clip and the path required to create the Media */
    std::map<int, std::string> m_Paths;
    /* Item indexes which are to be removed */
    std::vector<QModelIndex> m_Indexes;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_COMMANDS_H
