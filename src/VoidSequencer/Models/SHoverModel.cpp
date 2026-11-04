// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SHoverModel.h"

VOID_NAMESPACE_OPEN

SHoverModel::SHoverModel(QObject* parent)
    : QObject(parent)
    , m_Hovered(nullptr)
{
}

void SHoverModel::Set(const SharedTrackItem& item)
{
    // Same item again...
    if (item.get() == m_Hovered.get())
        return;

    SharedTrackItem prev = m_Hovered;
    m_Hovered = item;

    emit hoverChanged(m_Hovered, prev);
}

void SHoverModel::Reset()
{
    // Already reset...
    if (m_Hovered == nullptr)
        return;

    SharedTrackItem prev = m_Hovered;
    m_Hovered = nullptr;

    emit hoverChanged(m_Hovered, prev);
}

VOID_NAMESPACE_CLOSE
