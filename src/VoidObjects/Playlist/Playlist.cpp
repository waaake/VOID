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

bool Playlist::AddMedia(const SharedMediaClip& media)
{
    m_Media->Add(media);
    /* Update the modification state for the playlist */
    m_Modified = true;

    emit updated(this);
    return true;
}

bool Playlist::InsertMedia(const SharedMediaClip& media, const int index)
{
    m_Media->Insert(media, index);
    /* Update the modification state for the playlist */
    m_Modified = true;

    emit updated(this);
    return true;
}

bool Playlist::RemoveMedia(const QModelIndex& index)
{
    m_Media->Remove(index, false);
    /* Update the modification state for the playlist */
    m_Modified = true;

    emit updated(this);
    return true;
}

VOID_NAMESPACE_CLOSE
