// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_SELECTION_MODEL_H
#define _SEQUENCER_SELECTION_MODEL_H

/* STD */
#include <unordered_set>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

class SSelectionModel
{
public:
    void Clear();
    void Select(const SharedTrackItem& item);
    void Deselect(const SharedTrackItem& item);

    bool IsSelected(const SharedTrackItem& item);

private:
    std::unordered_set<SharedTrackItem> m_Selection;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_SELECTION_MODEL_H
