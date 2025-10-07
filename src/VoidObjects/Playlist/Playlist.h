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

/* Forward Decl for Project as the parent entity */
namespace Core {
    class Project;
} // namespace Core

class VOID_API Playlist : public VoidObject
{
    Q_OBJECT

public:
    Playlist(const std::string& name, Core::Project* parent);
    Playlist(Core::Project* parent);
    ~Playlist();

    inline bool Active() const { return m_Active; }
    inline void SetActive(bool active) { m_Active = active; }

    bool AddMedia(const SharedMediaClip& media);
    bool InsertMedia(const SharedMediaClip& media, const int index);
    bool RemoveMedia(const QModelIndex& index);

    inline MediaModel* DataModel() const { return m_Media; }
    const std::vector<SharedMediaClip> AllMedia() const { return m_Media->AllMedia(); }

    inline SharedMediaClip Media(const QModelIndex& index) const { return m_Media->Media(index); }
    inline SharedMediaClip Media(int row, int column) const { return m_Media->Media(m_Media->index(row, column)); }

    inline int Size() const { return static_cast<int>(m_Media->rowCount()); }
    inline bool Modified() const { return m_Modified; }

    std::string Name() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Serialize(std::ostream& out) const override;
    void Deserialize(const rapidjson::Value& in) override;
    void Deserialize(std::istream& in) override;
    inline const char* TypeName() const override { return "Playlist"; }

signals:
    void updated(const Playlist*);

protected: /* Members */
    MediaModel* m_Media;
    std::string m_Name;
    bool m_Modified;
    bool m_Active;
    Core::Project* m_Project;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYLIST_H
