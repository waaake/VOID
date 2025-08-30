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
    inline SharedMediaClip Media(int row, int column) const { return m_Media->Media(m_Media->index(row, column)); }

    inline MediaModel* DataModel() const { return m_Media; }
    inline QModelIndex ClipIndex(const SharedMediaClip& clip, int column = 0) const
    { 
        return m_Media->index(m_Media->MediaRow(clip), column); 
    }

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Deserialize(const rapidjson::Value& in) override;
    
    const char* TypeName() const override { return "Project"; }
    
    /**
     * Serialize the Project into a string which can be saved anywhere
     */
    std::string Document(const std::string& name) const;
    /**
     * The serialized string for the project can be used to construct the project from it
     */
    static Project* FromDocument(const std::string& document);

    /**
     * Save Processor: Saves the current State of the Project into the provided file
     * The provided name is the underlying name of the project to which it will be saved
     */
    bool Save();
    bool Save(const std::string& path, const std::string& name);

protected: /* Members */
    /* The Project holds the media and anything linking to the media */
    MediaModel* m_Media;

    /* Name of the Project */
    std::string m_Name;
    std::string m_Path;

    /* If the project is currently active */
    bool m_Active;
};

} // namespace Core

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_H
