// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_BASIC_COMMAND_H
#define _VOID_BASIC_COMMAND_H

/* Qt */
#include <QUndoCommand>

/* Internal */
#include "Definition.h"
#include "Status.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidUndoCommand : public QUndoCommand
{
public:
    VoidUndoCommand(QUndoCommand* parent = nullptr);
    void redo() override;

    /// @brief Returns whether the underlying action performed has been successful or not.
    /// @return CommandStatus for the performed action, is Unknown if the action hasn't been performed yer
    ///         else can be Success if command was performed successfully or Failure if it could not.
    CommandStatus Status() const { return m_Status; }

protected:
    virtual bool Redo() = 0;

private:
    CommandStatus m_Status;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_BASIC_COMMAND_H
