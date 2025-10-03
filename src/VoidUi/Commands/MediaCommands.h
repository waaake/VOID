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
#include "VoidUi/Media/MediaBridge.h"

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
    MediaRemoveCommand(const QModelIndex& index, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    std::string m_Path;
    QModelIndex m_Index;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_COMMANDS_H
