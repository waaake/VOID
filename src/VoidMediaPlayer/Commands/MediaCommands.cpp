// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Internal */
#include "MediaCommands.h"

VOID_NAMESPACE_OPEN

/// MediaImportCommand

MediaImportCommand::MediaImportCommand(Project* project, const std::string& path, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Path(MediaFS::ResolvedPath(path))
    , m_Project(project)
    , m_InsertIndex(project->DataModel()->MediaCount())
{
    setText("Import Media");
}

void MediaImportCommand::undo()
{
    QModelIndex index = m_Project->DataModel()->index(m_InsertIndex, 0);
    m_Project->Remove(index);
}

bool MediaImportCommand::Redo()
{
    return m_Project->AddMedia(MediaStruct::FromFile(m_Path));
}

/// MediaRemoveCommand

MediaRemoveCommand::MediaRemoveCommand(Project* project, const QModelIndex& index, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Project(project)
{
    setText("Remove Media");
}

void MediaRemoveCommand::undo()
{
    SharedMediaClip media = std::make_shared<MediaClip>(m_Project);
    std::istringstream is(m_Data, std::ios::binary);
    media->Deserialize(is);

    m_Project->InsertMedia(media, m_Index.row());
}

bool MediaRemoveCommand::Redo()
{
    std::ostringstream os(std::ios::binary);
    const SharedMediaClip& clip = m_Project->DataModel()->Media(m_Index);
    clip->Serialize(os);
    m_Data = os.str();

    return m_Project->Remove(m_Index);
}

/// AddSequenceCommand

AddSequenceCommand::AddSequenceCommand(Project* project, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Project(project)
    , m_InsertIndex(project->DataModel()->rowCount())
{
    setText("Add Sequence");
}

void AddSequenceCommand::undo()
{
    QModelIndex index = m_Project->DataModel()->index(m_InsertIndex, 0);
    m_Project->Remove(index); // Need to call it Remove
}

bool AddSequenceCommand::Redo()
{
    m_Project->Add(std::make_shared<PlaybackSequence>(m_Project));
    return true;
}

VOID_NAMESPACE_CLOSE
