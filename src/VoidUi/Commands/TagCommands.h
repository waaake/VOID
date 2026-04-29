// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TAG_COMMANDS_H
#define _TAG_COMMANDS_H

/* Internal */
#include "Definition.h"
#include "VoidCommand.h"
#include "VoidObjects/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

class ApplyTagCommand : public VoidUndoCommand
{
public:
    ApplyTagCommand(const QModelIndex& index, const std::string& tag, QUndoCommand* parent = nullptr);
    ApplyTagCommand(const QModelIndex& index, const std::string& tag, const TagMetaStruct& metadata, QUndoCommand* parent = nullptr);

    void undo() override;
    bool Redo() override;

private: /* Members */
    QModelIndex m_Index;
    std::string m_Tag;
    TagMetaStruct m_Metadata;
};

VOID_NAMESPACE_CLOSE

#endif // _TAG_COMMANDS_H
