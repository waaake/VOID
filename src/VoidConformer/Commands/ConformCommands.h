// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _CONFORM_COMMANDS_H
#define _CONFORM_COMMANDS_H

/* Internal */
#include "Definition.h"
#include "VoidUndo/VoidCommand.h"
#include "VoidObjects/Sequence/Context.h"

VOID_NAMESPACE_OPEN

class SetMediaCommand : public VoidUndoCommand
{
public:
    SetMediaCommand(const SharedMediaClip& clip, const SharedTrackItem& item, QUndoCommand* parent = nullptr);
    void undo() override;
    bool Redo() override;

private:
    Sequence::Context m_ItemContext;
    Core::Project* m_Project;
    int m_MediaIndex;
    int m_PreviousIndex;
};

VOID_NAMESPACE_CLOSE

#endif // _CONFORM_COMMANDS_H
