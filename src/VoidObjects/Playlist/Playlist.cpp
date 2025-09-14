// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Playlist.h"

VOID_NAMESPACE_OPEN

Playlist::Playlist(const std::string& name, QObject* parent)
    : VoidObject(parent)
    , m_Name(name)
    , m_Modified(false)
{
    m_Media = new MediaModel(this);
}

Playlist::Playlist(QObject* parent)
    : Playlist("Untitled", parent)
{
}

Playlist::~Playlist()
{
    /* Remove Any references to the Shared pointer for MediaClip */
    for (SharedMediaClip& clip : *m_Media)
        clip = nullptr;

    m_Media->Clear();
}

void Playlist::AddMedia(const SharedMediaClip& media)
{
    m_Media->Add(media);
    /* Update the modification state for the playlist */
    m_Modified = true;
}

void Playlist::InsertMedia(const SharedMediaClip& media, const int index)
{
    m_Media->Insert(media, index);
    /* Update the modification state for the playlist */
    m_Modified = true;
}

void Playlist::RemoveMedia(const QModelIndex& index)
{
    m_Media->Remove(index);
    /* Update the modification state for the playlist */
    m_Modified = true;
}

VOID_NAMESPACE_CLOSE
