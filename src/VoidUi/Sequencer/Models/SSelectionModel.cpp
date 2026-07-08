// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SSelectionModel.h"

VOID_NAMESPACE_OPEN

void SSelectionModel::Clear()
{
    m_Selection.clear();
}

void SSelectionModel::Select(const SharedTrackItem& item)
{
    m_Selection.insert(item);
}

void SSelectionModel::Deselect(const SharedTrackItem& item)
{
    m_Selection.erase(item);
}

bool SSelectionModel::IsSelected(const SharedTrackItem& item)
{
    return m_Selection.find(item) != m_Selection.end();
}

VOID_NAMESPACE_CLOSE
