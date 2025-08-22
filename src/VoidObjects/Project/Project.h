// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PROJECT_H
#define _VOID_PROJECT_H

/* Internal */
#include "Definition.h"
#include "VoidObjects/VoidObject.h"
#include "VoidObjects/Models/MediaModel.h"

VOID_NAMESPACE_OPEN

namespace Core {

/**
 * A Project is essentially, just the store for Media and Media entities or collection
 * like Sequence and/or Playlist, it does not do anything apart from holding references
 * to the data via the MediaModel
 */
class VOID_API Project : public VoidObject
{
public:
    Project(bool active = true, QObject* parent = nullptr);
    Project(const std::string& name, bool active = true, QObject* parent = nullptr);

    virtual ~Project();

    inline const std::string& Name() const { return m_Name; }

    inline bool Active() const { return m_Active; }
    inline void SetActive(bool active) { m_Active = active; }
    
    inline void AddMedia(const SharedMediaClip& media) { m_Media->Add(media); }
    inline void InsertMedia(const SharedMediaClip& media, const int index) { m_Media->Insert(media, index); }
    inline void RemoveMedia(const QModelIndex& index) { m_Media->Remove(index); }
    inline SharedMediaClip Media(const QModelIndex& index) const { return m_Media->Media(index); }

    inline MediaModel* DataModel() const { return m_Media; }
    inline QModelIndex ClipIndex(const SharedMediaClip& clip, int column = 0) const
    { 
        return m_Media->index(m_Media->MediaRow(clip), column); 
    }

protected: /* Members */
    /* The Project holds the media and anything linking to the media */
    MediaModel* m_Media;

    /* Name of the Project */
    std::string m_Name;

    /* If the project is currently active */
    bool m_Active;
};

} // namespace Core

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_H
