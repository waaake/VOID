// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "EffectCommands.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidObjects/Effects/Effects.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

CreateEffectCommand::CreateEffectCommand(const QModelIndex& index, const std::string& effect, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Index(index)
    , m_Effect(effect)
{
    setText("Create Effect");
}

void CreateEffectCommand::undo()
{
    if (SharedMediaClip media = _MediaBridge.MediaAt(m_Index))
        media->RemoveEffect(m_Name);
}

bool CreateEffectCommand::Redo()
{
    if (SharedMediaClip media = _MediaBridge.MediaAt(m_Index))
    {
        if (auto effect = media->CreateEffect(m_Effect))
        {
            if (m_Name.empty())
                m_Name = effect->Name();
            else
                effect->SetName(m_Name);

            return true;
        }
    }

    return false;
}

VOID_NAMESPACE_CLOSE
