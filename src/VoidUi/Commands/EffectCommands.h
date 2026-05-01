// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _EFFECT_COMMANDS_H
#define _EFFECT_COMMANDS_H

/* Qt */
#include <QModelIndex>

/* Internal */
#include "Definition.h"
#include "VoidCommand.h"

VOID_NAMESPACE_OPEN

class CreateEffectCommand : public VoidUndoCommand
{
public:
    CreateEffectCommand(const QModelIndex& index, const std::string& effect, QUndoCommand* parent = nullptr);

    void undo();
    bool Redo();

private: /* Members */
    QModelIndex m_Index;
    std::string m_Effect;
    std::string m_Name;
};

VOID_NAMESPACE_CLOSE

#endif // _EFFECT_COMMANDS_H
