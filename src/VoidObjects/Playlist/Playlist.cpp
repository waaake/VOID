// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Playlist.h"
#include "VoidObjects/Project/Project.h"

VOID_NAMESPACE_OPEN

Playlist::Playlist(const std::string& name, Core::Project* parent)
    : VoidObject(parent)
    , m_Name(name)
    , m_Modified(false)
    , m_Project(parent)
    , m_CurrentRow(0)
{
    m_Media = new MediaModel(this);
}

Playlist::Playlist(Core::Project* parent)
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

void Playlist::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();
    out.AddMember("type", rapidjson::Value(TypeName(), allocator), allocator);
    out.AddMember("name", rapidjson::Value(m_Name.c_str(), allocator), allocator);
    
    rapidjson::Value media(rapidjson::kArrayType);

    for (const SharedMediaClip& clip : *m_Media)
    {
        rapidjson::Value clipObject;
        clipObject.SetObject();

        clipObject.AddMember("row", m_Project->DataModel()->MediaRow(clip), allocator);
        media.PushBack(clipObject, allocator);
    }

    out.AddMember("Clips", media, allocator);
}

void Playlist::Serialize(std::ostream& out) const
{
    WriteString(out, m_Name);
    uint32_t count = static_cast<uint32_t>(m_Media->rowCount());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const SharedMediaClip& clip : *m_Media)
    {
        int row = m_Project->DataModel()->MediaRow(clip);
        out.write(reinterpret_cast<const char*>(&row), sizeof(row));
    }
}

void Playlist::Deserialize(const rapidjson::Value& in)
{
    m_Name = in["name"].GetString();
    const rapidjson::Value::ConstArray media = in["Clips"].GetArray();

    for (int i = 0; i < media.Size(); ++i)
        m_Media->Add(m_Project->MediaAt(media[i]["row"].GetInt(), 0));

    emit updated(this);
}

void Playlist::Deserialize(std::istream& in)
{
    m_Name = ReadString(in);

    uint32_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (uint32_t i = 0; i < count; ++i)
    {
        int row;
        in.read(reinterpret_cast<char*>(&row), sizeof(row));
        m_Media->Add(m_Project->MediaAt(row, 0));
    }

    emit updated(this);
}

SharedMediaClip Playlist::NextMedia()
{
    m_CurrentRow = m_CurrentRow == m_Media->rowCount() - 1 ? 0 : ++m_CurrentRow;
    return m_Media->Media(m_Media->index(m_CurrentRow, 0));
}

SharedMediaClip Playlist::PreviousMedia()
{
    m_CurrentRow = m_CurrentRow == 0 ? m_Media->rowCount() - 1 : --m_CurrentRow;    
    return m_Media->Media(m_Media->index(m_CurrentRow, 0));
}

VOID_NAMESPACE_CLOSE
