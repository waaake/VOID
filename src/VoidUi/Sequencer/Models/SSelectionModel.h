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
#include "VoidObjects/Effects/Effects.h"

VOID_NAMESPACE_OPEN

class SSelectionModel : public QObject
{
    Q_OBJECT
public:
    void Clear();
    void Select(const SharedTrackItem& item);
    void Select(const std::vector<SharedTrackItem>& items);
    void Select(const SharedPlaybackTrack& track);
    void Select(Effect* effect);
    void Select(const std::vector<Effect*>& effects);
    void Deselect(const SharedTrackItem& item);
    void Deselect(const SharedPlaybackTrack& track);
    void Deselect(Effect* effect);
    void Toggle(const SharedTrackItem& item);
    void Toggle(const SharedPlaybackTrack& track);
    void Toggle(Effect* effect);

    bool IsSelected(const TrackItem* item);
    bool IsSelected(const SharedTrackItem& item);
    bool IsSelected(const SharedPlaybackTrack& track);
    bool IsSelected(Effect* effect);
    bool HasTrackItemSelection() const { return !m_Items.empty(); }
    bool HasEffectSelection() const { return !m_Effects.empty(); }
    bool HasTrackSelection() const { return !m_Tracks.empty(); }
    const std::unordered_set<SharedTrackItem>& SelectedItems() const { return m_Items; }
    const std::unordered_set<SharedPlaybackTrack>& SelectedTracks() const { return m_Tracks; }
    const std::unordered_set<Effect*>& SelectedEffects() const { return m_Effects; }

signals:
    void selectionChanged();
    void trackSelectionChanged();
    void effectSelectionChanged();

private:
    std::unordered_set<SharedTrackItem> m_Items;
    std::unordered_set<Effect*> m_Effects;
    std::unordered_set<SharedPlaybackTrack> m_Tracks;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_SELECTION_MODEL_H
