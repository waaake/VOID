// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PROJECT_H
#define _VOID_PROJECT_H

/* Internal */
#include "Definition.h"
#include "VoidObjects/VoidObject.h"
#include "VoidObjects/Models/EntityModel.h"
#include "VoidObjects/Models/PlaylistModel.h"

VOID_NAMESPACE_OPEN

namespace Core {

/**
 * A Project is essentially, just the store for Media and Media entities or collection
 * like Sequence and/or Playlist, it does not do anything apart from holding references
 * to the data via the media/other entities' EntityModel
 */
class VOID_API Project : public VoidObject
{
    Q_OBJECT
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
    /**
     * Returns whether the project has yet been saved and us currently not in modified state
     * Project being referred to as saved means that it has a physical file location
     */
    inline bool Saved() const { return !m_Modified && !m_Path.empty(); }

    const std::vector<SharedMediaClip>& MediaClips() const { return m_Media->MediaClips(); }
    const std::vector<SharedPlaybackSequence>& Sequences() const { return m_Media->Sequences(); }
    bool AddMedia(MediaStruct&& mstruct);
    bool AddMedia(const MediaStruct& mstruct);
    bool InsertMedia(MediaStruct&& mstruct, int index);
    bool InsertMedia(const MediaStruct& mstruct, int index);
    bool InsertMedia(const SharedMediaClip& media, int index);
    
    void Add(const SharedMediaClip& media);
    void Add(const SharedPlaybackSequence& media);
    void Insert(const SharedMediaClip& media, const int index);
    void Insert(const SharedPlaybackSequence& sequence, const int index);
    bool Remove(const QModelIndex& index);
    bool RemoveMedia(const QModelIndex& index);
    bool RemoveSequence(const QModelIndex& index);

    inline SharedMediaClip MediaAt(const QModelIndex& index) const { return m_Media->Media(index); }
    inline SharedMediaClip MediaAt(int row, int column) const { return m_Media->Media(m_Media->index(row, column)); }
    inline SharedMediaClip LastMedia() const { return m_Media->LastMedia(); }
    SharedMediaClip PlaylistMediaAt(const QModelIndex& index) const;
    SharedMediaClip PlaylistMediaAt(int row, int column) const;

    int SequenceRow(const PlaybackSequence* sequence) const { return m_Media->SequenceRow(sequence); }
    int SequenceRow(const SharedPlaybackSequence& sequence) const { return m_Media->SequenceRow(sequence); }
    SharedPlaybackSequence Sequence(const QModelIndex& index) const { return m_Media->Sequence(index); }
    SharedPlaybackSequence Sequence(int row) const { return m_Media->Sequence(row); }

    inline EntityModel* DataModel() const { return m_Media; }
    inline PlaylistModel* PlaylistMediaModel() const { return m_Playlists; }

    inline QModelIndex ClipIndex(const SharedMediaClip& clip, int column = 0) const
    { 
        return m_Media->index(m_Media->MediaRow(clip), column); 
    }
    int MediaRow(const SharedMediaClip& clip) const { return m_Media->MediaRow(clip); }

    Playlist* NewPlaylist();
    Playlist* NewPlaylist(const std::string& name);
    Playlist* NewPlaylist(const std::string& name, int index);
    void InsertPlaylist(Playlist* playlist, int index);
    inline Playlist* ActivePlaylist() const { return m_Playlist; }
    inline Playlist* PlaylistAt(const QModelIndex& index) const { return m_Playlists->PlaylistAt(index); }
    inline Playlist* PlaylistAt(int row, int column) const { return m_Playlists->PlaylistAt(row, column); }
    inline void RefreshPlaylist() { m_Playlists->Refresh(); }
    void SetCurrentPlaylist(const QModelIndex& index);
    void SetCurrentPlaylist(int index);
    void RemovePlaylist(const QModelIndex& index);

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
    bool Save(const std::string& path, const std::string& name, const EtherFormat::Type& type);
    
    /**
     * Update the path for the project on which it will save to
     * if this is defined, the project can directly be saved without explicitly providing a path to save onto
     */
    inline void SetSavePath(const std::string& path) { m_Path = path; }

signals:
    void mediaAdded(const SharedMediaClip&);
    void mediaAboutToBeRemoved(const SharedMediaClip&);
    void mediaRemoved();
    void sequenceAdded(const SharedPlaybackSequence&);
    void sequenceAboutToBeRemoved(const SharedPlaybackSequence&);
    void sequenceRemoved();
    void playlistCreated(const Playlist*);
    void playlistChanged(const Playlist*);

protected: /* Members */
    EntityModel* m_Media;
    PlaylistModel* m_Playlists;
    Playlist* m_Playlist;

    std::string m_Name;
    std::string m_Path;
    EtherFormat::Type m_Type;

    bool m_Active;
    bool m_Modified;

private: /* Methods */
    void SetActivePlaylist(Playlist* playlist);
    bool SaveInternal(const std::string& path, const std::string& name, const EtherFormat::Type& type);
    bool SaveAscii(const std::string& path, const std::string& name);
    bool SaveBinary(const std::string& path, const std::string& name);
};

} // namespace Core

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_H
