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
    std::string m_Path;
    int m_InsertIndex;
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

class AddSequenceCommand : public VoidUndoCommand
{
public:
    AddSequenceCommand(Project* project, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private:
    Project* m_Project;
    int m_InsertIndex;    
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_COMMANDS_H
