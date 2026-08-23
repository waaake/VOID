// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_COMMANDS_H
#define _VOID_MEDIA_COMMANDS_H

/* STD */
#include <map>
#include <vector>

/* Internal */
#include "Definition.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"
#include "VoidUndo/VoidCommand.h"

VOID_NAMESPACE_OPEN

class MediaImportCommand : public VoidUndoCommand
{
public:
    MediaImportCommand(const std::string& path, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    int m_InsertIndex;
    std::string m_Path;
};

class MediaRemoveCommand : public VoidUndoCommand
{
public:
    MediaRemoveCommand(const QModelIndex& index, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    std::string m_Data;
    QModelIndex m_Index;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_COMMANDS_H
