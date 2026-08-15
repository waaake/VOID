// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "VoidCommand.h"

VOID_NAMESPACE_OPEN

VoidUndoCommand::VoidUndoCommand(QUndoCommand* parent)
    : QUndoCommand(parent)
{
}

void VoidUndoCommand::redo()
{
    /**
     * If the Underlying Redo function returns False
     * this means that the command failed to execute and cannot be undone or redone again
     * marking this as obsolete so that this stays out of the undo stack
     */
    bool status = Redo();

    setObsolete(!status);
}

VOID_NAMESPACE_CLOSE
