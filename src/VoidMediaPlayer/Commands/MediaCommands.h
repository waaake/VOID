// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_COMMANDS_H
#define _VOID_MEDIA_COMMANDS_H

/* STD */
#include <map>
#include <vector>

/* Internal */
#include "Definition.h"
#include "VoidMediaPlayer/Project/Project.h"
#include "VoidUndo/VoidCommand.h"

VOID_NAMESPACE_OPEN

class MediaImportCommand : public VoidUndoCommand
{
public:
    MediaImportCommand(Project* project, const std::string& path, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    std::string m_Path;
    Project* m_Project;
    int m_InsertIndex;
};

class RemoveEntityCommand : public VoidUndoCommand
{
public:
    RemoveEntityCommand(Project* project, const QModelIndex& index, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    std::string m_Data;
    QModelIndex m_Index;
    Project* m_Project;
    ProjectEntity::Type m_Type;
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
