// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ConformCommands.h"

VOID_NAMESPACE_OPEN

SetMediaCommand::SetMediaCommand(const SharedMediaClip& media, const SharedTrackItem& item, QUndoCommand* parent)
    : m_ItemContext(Sequence::Context::Get(item))
    , m_Project(media->Project())
    , m_MediaIndex(m_Project->MediaRow(media))
    , m_PreviousIndex(-1)
{
    setText("Relink Media");
}

void SetMediaCommand::undo()
{
    Sequence::ResolvedContext context = m_ItemContext.Resolve();
    if (m_PreviousIndex > -1)
        return context.trackItem->ResetMedia(m_Project->MediaAt(m_PreviousIndex, 0));

    context.trackItem->Unlink();
}

bool SetMediaCommand::Redo()
{
    Sequence::ResolvedContext context = m_ItemContext.Resolve();
    if (context.trackItem->Linked())
        m_PreviousIndex = m_Project->MediaRow(context.trackItem->GetMedia());

    context.trackItem->ResetMedia(m_Project->MediaAt(m_MediaIndex, 0));
    return true;
}

VOID_NAMESPACE_CLOSE
