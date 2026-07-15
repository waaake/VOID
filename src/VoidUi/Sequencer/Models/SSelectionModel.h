// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_SELECTION_MODEL_H
#define _SEQUENCER_SELECTION_MODEL_H

/* STD */
#include <unordered_set>
#include <vector>

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

class SSelectionModel : public QObject
{
    Q_OBJECT
public:
    void Clear();
    void Select(const SharedTrackItem& item);
    void Select(const std::vector<SharedTrackItem>& items);
    void Deselect(const SharedTrackItem& item);
    void Toggle(const SharedTrackItem& item);

    bool IsSelected(const SharedTrackItem& item);
    const std::unordered_set<SharedTrackItem>& Current() const { return m_Selection; }

signals:
    void selectionChanged();

private:
    std::unordered_set<SharedTrackItem> m_Selection;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_SELECTION_MODEL_H
