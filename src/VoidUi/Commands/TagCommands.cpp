// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "TagCommands.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

ApplyTagCommand::ApplyTagCommand(const QModelIndex& index, const std::string& tag, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Tag(tag)
{
    setText("Add Tag");
}

ApplyTagCommand::ApplyTagCommand(const QModelIndex& index, const std::string& tag, const TagMetaStruct& metadata, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Tag(tag)
    , m_Metadata(metadata)
{
    setText("Add Tag");
}

void ApplyTagCommand::undo()
{
    if (SharedMediaClip media = _MediaBridge.MediaAt(m_Index))
        media->RemoveTag(static_cast<int>(media->Tags().size()) - 1);
}

bool ApplyTagCommand::Redo()
{
    if (SharedMediaClip media = _MediaBridge.MediaAt(m_Index))
        return m_Metadata.empty() ? media->AddTag(m_Tag) : media->AddTag(m_Tag, m_Metadata);

    return false;
}

VOID_NAMESPACE_CLOSE
