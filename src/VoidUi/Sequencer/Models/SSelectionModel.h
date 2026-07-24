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
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

class SSelectionModel : public QObject
{
    Q_OBJECT
public:
    void Clear();
    void Select(const SharedTrackItem& item);
    void Select(const std::vector<SharedTrackItem>& items);
    void Select(const SharedPlaybackTrack& track);
    void Deselect(const SharedTrackItem& item);
    void Deselect(const SharedPlaybackTrack& track);
    void Toggle(const SharedTrackItem& item);
    void Toggle(const SharedPlaybackTrack& track);

    bool IsSelected(const SharedTrackItem& item);
    bool IsSelected(const SharedPlaybackTrack& track);
    bool HasSelection() const { return !m_Items.empty(); }
    bool HasTrackSelection() const { return !m_Tracks.empty(); }
    const std::unordered_set<SharedTrackItem>& SelectedItems() const { return m_Items; }
    const std::unordered_set<SharedPlaybackTrack>& SelectedTracks() const { return m_Tracks; }

signals:
    void selectionChanged();
    void trackSelectionChanged();

private:
    std::unordered_set<SharedTrackItem> m_Items;
    std::unordered_set<SharedPlaybackTrack> m_Tracks;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_SELECTION_MODEL_H
