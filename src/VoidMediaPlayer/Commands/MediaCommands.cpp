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
    , m_Project(project)
    , m_Path(MediaFS::ResolvedPath(path))
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

MediaRemoveCommand::MediaRemoveCommand(const QModelIndex& index, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
{
    setText("Remove Media");
}

void MediaRemoveCommand::undo()
{
    SharedMediaClip media = std::make_shared<MediaClip>(_MediaBridge.ActiveProject());
    std::istringstream is(m_Data, std::ios::binary);
    media->Deserialize(is);

    _MediaBridge.InsertMedia(media, m_Index.row());
}

bool MediaRemoveCommand::Redo()
{
    std::ostringstream os(std::ios::binary);
    const SharedMediaClip& clip = _MediaBridge.DataModel()->Media(m_Index);
    clip->Serialize(os);
    m_Data = os.str();

    return _MediaBridge.Remove(m_Index);
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
