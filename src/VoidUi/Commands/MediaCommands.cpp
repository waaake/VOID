// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MediaCommands.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* Media Import Command {{{ */

MediaImportCommand::MediaImportCommand(const std::string& path, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Path(MediaFS::ResolvedPath(path))
{
    /* The current index on which the Media Will be inserted */
    m_InsertIndex = _MediaBridge.DataModel()->rowCount();

    setText("Import Media");
}

void MediaImportCommand::undo()
{
    /* Index at which the Media was inserted and now needs removal */
    QModelIndex index = _MediaBridge.DataModel()->index(m_InsertIndex, 0);

    /* Remove the Media at the index */
    _MediaBridge.Remove(index);
}

bool MediaImportCommand::Redo()
{
    /* Construct the media struct from the file path */
    MediaStruct mstruct = MediaStruct::FromFile(m_Path);

    /* Validate before adding */
    if (mstruct.Empty())
    {
        VOID_LOG_INFO("Invalid Media");
        return false;
    }

    if (!mstruct.ValidMedia())
    {
        VOID_LOG_INFO("Invalid Media: {0}", mstruct.FirstPath());
        return false;
    }

    /* Add the Media to the Model */
    return _MediaBridge.AddMedia(mstruct);
}

/* }}} */

/* Media Remove Command {{{ */

MediaRemoveCommand::MediaRemoveCommand(const QModelIndex& index, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
{
    setText("Remove Media");
}

void MediaRemoveCommand::undo()
{
    MediaStruct mstruct = MediaStruct::FromFile(m_Path);

    /* Validate before adding */
    if (mstruct.Empty())
    {
        VOID_LOG_INFO("Invalid Media");
        return;
    }

    if (!mstruct.ValidMedia())
    {
        VOID_LOG_INFO("Invalid Media: {0}", mstruct.FirstPath());
        return;
    }

    /* Add the Media to the Model */
    _MediaBridge.InsertMedia(mstruct, m_Index.row());
}

bool MediaRemoveCommand::Redo()
{
    /* Update the internal struct so that we know where each clip was and it's path */
    const SharedMediaClip clip = _MediaBridge.DataModel()->Media(m_Index);
    m_Path = clip->FirstFrameData().Path();

    return _MediaBridge.Remove(m_Index);
}

/* }}} */

VOID_NAMESPACE_CLOSE
