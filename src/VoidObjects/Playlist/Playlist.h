// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYLIST_H
#define _VOID_PLAYLIST_H

/* STD */
#include <vector>

/* Internal */
#include "Definition.h"
#include "VoidObjects/VoidObject.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Models/MediaModel.h"

VOID_NAMESPACE_OPEN

class VOID_API Playlist : public VoidObject
{
    Q_OBJECT

public:
    Playlist(const std::string& name, QObject* parent = nullptr);
    Playlist(QObject* parent = nullptr);
    ~Playlist();

    inline bool Active() const { return m_Active; }
    inline void SetActive(bool active) { m_Active = active; }

    void AddMedia(const SharedMediaClip& media);
    void InsertMedia(const SharedMediaClip& media, const int index);
    void RemoveMedia(const QModelIndex& index);

    inline MediaModel* DataModel() const { return m_Media; }

    inline SharedMediaClip Media(const QModelIndex& index) const { return m_Media->Media(index); }
    inline SharedMediaClip Media(int row, int column) const { return m_Media->Media(m_Media->index(row, column)); }

    inline int MediaCount() const { return static_cast<int>(m_Media->rowCount()); }
    inline bool Modified() const { return m_Modified; }

    std::string Name() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }

signals:
    void updated(const Playlist*);

protected: /* Members */
    MediaModel* m_Media;
    std::string m_Name;
    bool m_Modified;
    bool m_Active;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYLIST_H
