// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_BASIC_COMMAND_H
#define _VOID_BASIC_COMMAND_H

/* Qt */
#include <QUndoCommand>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VoidUndoCommand : public QUndoCommand
{
public:
    VoidUndoCommand(QUndoCommand* parent = nullptr);
    /* Override the base Redo to set Obsolete state when the Redo function return false */
    void redo() override;

protected:
    virtual bool Redo() = 0;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_BASIC_COMMAND_H
