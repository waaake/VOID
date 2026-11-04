// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm> // std::find_if

/* Internal */
#include "SSelectionModel.h"

VOID_NAMESPACE_OPEN

void SSelectionModel::Clear()
{
    m_Items.clear();
    m_Tracks.clear();
    m_Effects.clear();
    emit selectionChanged();
    emit trackSelectionChanged();
    emit effectSelectionChanged();
}

void SSelectionModel::Select(const SharedTrackItem& item)
{
    m_Items.insert(item);
    emit selectionChanged();
}

void SSelectionModel::Select(const std::vector<SharedTrackItem>& items)
{
    m_Items.clear();
    m_Items.reserve(items.size());
    for (auto& item : items)
        m_Items.insert(item);

    emit selectionChanged();
}

void SSelectionModel::Select(const SharedPlaybackTrack& track)
{
    m_Tracks.insert(track);
    emit trackSelectionChanged();
}

void SSelectionModel::Select(Effect* effect)
{
    m_Effects.insert(effect);
    emit effectSelectionChanged();
}

void SSelectionModel::Select(const std::vector<Effect*>& effects)
{
    m_Effects.clear();
    m_Effects.reserve(effects.size());

    for (auto& effect : effects)
        m_Effects.insert(effect);

    emit effectSelectionChanged();
}

void SSelectionModel::Deselect(const SharedTrackItem& item)
{
    m_Items.erase(item);
    emit selectionChanged();
}

void SSelectionModel::Deselect(const SharedPlaybackTrack& track)
{
    m_Tracks.erase(track);
    emit trackSelectionChanged();
}

void SSelectionModel::Deselect(Effect* effect)
{
    m_Effects.erase(effect);
    emit effectSelectionChanged();
}

void SSelectionModel::Toggle(const SharedTrackItem& item)
{
    if (m_Items.find(item) == m_Items.end())
        m_Items.insert(item);
    else
        m_Items.erase(item);

    emit selectionChanged();
}

void SSelectionModel::Toggle(const SharedPlaybackTrack& track)
{
    if (m_Tracks.find(track) == m_Tracks.end())
        m_Tracks.insert(track);
    else
        m_Tracks.erase(track);

    emit trackSelectionChanged();
}

void SSelectionModel::Toggle(Effect* effect)
{
    if (m_Effects.find(effect) == m_Effects.end())
        m_Effects.insert(effect);
    else
        m_Effects.erase(effect);

    emit effectSelectionChanged();
}

bool SSelectionModel::IsSelected(const TrackItem* item)
{
    if (!item) return false;

    auto it = std::find_if(
        m_Items.begin(),
        m_Items.end(),
        [item](const SharedTrackItem& _i) -> bool
        {
            return _i.get() == item;
        }
    );

    return it != m_Items.end();
}

bool SSelectionModel::IsSelected(const SharedTrackItem& item)
{
    return m_Items.find(item) != m_Items.end();
}

bool SSelectionModel::IsSelected(const SharedPlaybackTrack& track)
{
    return m_Tracks.find(track) != m_Tracks.end();
}

bool SSelectionModel::IsSelected(Effect* effect)
{
    return m_Effects.find(effect) != m_Effects.end();
}

VOID_NAMESPACE_CLOSE
