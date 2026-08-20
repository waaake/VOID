// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Internal */
#include "TagCommands.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

/// ApplyTagCommand

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

/// RemoveTagCommand

RemoveTagCommand::RemoveTagCommand(const QModelIndex& mindex, const QModelIndex& tindex, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_MediaIndex(mindex)
    , m_TagIndex(tindex)
{
    setText("Remove Tag");
}

void RemoveTagCommand::undo()
{
    if (SharedMediaClip media = _MediaBridge.MediaAt(m_MediaIndex))
    {
        std::istringstream in(m_TagData, std::ios::binary);
        Tag* tag = new Tag("t");
        tag->Deserialize(in);

        media->InsertTag(tag, m_TagIndex.row());
    }
}

bool RemoveTagCommand::Redo()
{
    if (SharedMediaClip media = _MediaBridge.MediaAt(m_MediaIndex))
    {
        if (Tag* tag = media->TagAt(m_TagIndex))
        {
            std::ostringstream os(std::ios::binary);
            tag->Serialize(os);

            m_TagData = os.str();

            // We're going to Remove the same tag here, so Tag* tag will be a dangling pointer to something
            // which gets deleted after this call, make sure we don't have a case for that
            tag = nullptr;
            media->RemoveTag(m_TagIndex);

            return true;
        }
    }

    return false;
}

VOID_NAMESPACE_CLOSE
