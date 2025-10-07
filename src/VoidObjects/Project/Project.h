// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PROJECT_H
#define _VOID_PROJECT_H

/* Internal */
#include "Definition.h"
#include "VoidObjects/VoidObject.h"
#include "VoidObjects/Models/MediaModel.h"
#include "VoidObjects/Models/PlaylistModel.h"

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
   
    /**
     * Returns whether any changes have been made to the project since it was last saved
     * (or hasn't been saved at all) or modified since it was created
     */
    inline bool Modified() const { return m_Modified; }
    
    void AddMedia(const SharedMediaClip& media);
    void InsertMedia(const SharedMediaClip& media, const int index);
    void RemoveMedia(const QModelIndex& index);

    inline SharedMediaClip MediaAt(const QModelIndex& index) const { return m_Media->Media(index); }
    inline SharedMediaClip MediaAt(int row, int column) const { return m_Media->Media(m_Media->index(row, column)); }
    SharedMediaClip PlaylistMediaAt(const QModelIndex& index) const;
    SharedMediaClip PlaylistMediaAt(int row, int column) const;

    inline MediaModel* DataModel() const { return m_Media; }
    inline PlaylistModel* PlaylistMediaModel() const { return m_Playlists; }

    inline QModelIndex ClipIndex(const SharedMediaClip& clip, int column = 0) const
    { 
        return m_Media->index(m_Media->MediaRow(clip), column); 
    }

    inline Playlist* ActivePlaylist() const { return m_Playlist; }
    inline Playlist* PlaylistAt(const QModelIndex& index) const { return m_Playlists->PlaylistAt(index); }
    inline Playlist* PlaylistAt(int row, int column) const { return m_Playlists->PlaylistAt(row, column); }
    inline void RefreshPlaylist() { m_Playlists->Refresh(); }

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Serialize(std::ostream& out) const override;
    void Deserialize(const rapidjson::Value& in) override;
    void Deserialize(std::istream& in) override;
    inline const char* TypeName() const override { return "Project"; }
    
    /**
     * Serialize the Project into a string which can be saved anywhere
     */
    std::string Document(const std::string& name) const;
    /**
     * The serialized string for the project can be used to construct the project from it
     */
    static Project* FromDocument(const std::string& document);

    /**
     * Serialize the Project into an output stream which can be saved anywhere
     */
    void ToStream(std::ostream& out, const std::string& name) const;
    /**
     * Construct the Project from the input stream of the data
     */
    static Project* FromStream(std::istream& in);
    
    /**
     * Save Processor: Saves the current State of the Project into the provided file
     * The provided name is the underlying name of the project to which it will be saved
     */
    bool Save();
    inline bool Save(const std::string& path, const std::string& name, const EtherFormat::Type& type) { return SaveInternal(path, name, type); }
    
    /**
     * Update the path for the project on which it will save to
     * if this is defined, the project can directly be saved without explicitly providing a path to save onto
     */
    inline void SetSavePath(const std::string& path) { m_Path = path; }

protected: /* Members */
    /* The Project holds the media and anything linking to the media */
    MediaModel* m_Media;
    /* Any Playlists in the project */
    PlaylistModel* m_Playlists;
    /* Currently Active Playlist */
    Playlist* m_Playlist;

    /* Project Descriptors */
    std::string m_Name;
    std::string m_Path;
    EtherFormat::Type m_Type;

    /* If the project is currently active */
    bool m_Active;
    bool m_Modified;

private: /* Methods */
    bool SaveInternal(const std::string& path, const std::string& name, const EtherFormat::Type& type);
    bool SaveAscii(const std::string& path, const std::string& name);
    bool SaveBinary(const std::string& path, const std::string& name);
};

} // namespace Core

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_H
