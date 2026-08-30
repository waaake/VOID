// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <fstream>

/* RapidJSON */
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

/* Internal */
#include "Project.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

namespace Core {

Project::Project(const std::string& name, bool active, QObject* parent)
    : VoidObject(parent)
    , m_Playlist(nullptr)
    , m_Name(name)
    , m_Path("")
    , m_Type(EtherFormat::Type::ASCII)
    , m_Active(active)
    , m_Modified(false)
{
    m_Media = new EntityModel(this);
    m_Playlists = new PlaylistModel(this);
    
    connect(m_Media, &EntityModel::updated, this, [this]() -> void { m_Modified = true; });
    VOID_LOG_INFO("Project {0} Created: {1}", name, Vuid());
}

Project::Project(bool active, QObject* parent)
    : Project("Untitled", active, parent)
{
}

Project::~Project()
{
    m_Media->deleteLater();
    delete m_Media;
    m_Media = nullptr;

    m_Playlists->deleteLater();
    delete m_Playlists;
    m_Playlists = nullptr;
}

bool Project::AddMedia(MediaStruct&& mstruct)
{
    if (mstruct.Empty())
    {
        VOID_LOG_INFO("Invalid Media");
        return false;
    }

    if (!mstruct.ValidMedia())
    {
        VOID_LOG_INFO("Invalid Media: {0}", mstruct.FirstPath());
        return false;
    }

    SharedMediaClip clip = std::make_shared<MediaClip>(mstruct, this);
    if (clip->Valid())
    {
        m_Media->Add(clip);
        emit mediaAdded(clip);
        // Success
        return true;
    }

    VOID_LOG_INFO("Invalid Media Type");
    return false;
}

bool Project::AddMedia(const MediaStruct& mstruct)
{
    if (mstruct.Empty())
    {
        VOID_LOG_INFO("Invalid Media");
        return false;
    }

    if (!mstruct.ValidMedia())
    {
        VOID_LOG_INFO("Invalid Media: {0}", mstruct.FirstPath());
        return false;
    }

    SharedMediaClip clip = std::make_shared<MediaClip>(mstruct, this);
    if (clip->Valid())
    {
        m_Media->Add(clip);
        emit mediaAdded(clip);
        // Success
        return true;
    }

    VOID_LOG_INFO("Invalid Media Type");
    return false;
}

bool Project::InsertMedia(MediaStruct&& mstruct, int index)
{
    if (mstruct.Empty())
    {
        VOID_LOG_INFO("Invalid Media");
        return false;
    }

    if (!mstruct.ValidMedia())
    {
        VOID_LOG_INFO("Invalid Media: {0}", mstruct.FirstPath());
        return false;
    }

    SharedMediaClip clip = std::make_shared<MediaClip>(mstruct, this);
    if (clip->Valid())
    {
        m_Media->Insert(clip, index);
        emit mediaAdded(clip);
        // Success
        return true;
    }

    VOID_LOG_INFO("Invalid Media Type");
    return false;
}

bool Project::InsertMedia(const MediaStruct& mstruct, int index)
{
    SharedMediaClip clip = std::make_shared<MediaClip>(mstruct, this);
    if (clip->Valid())
    {
        m_Media->Insert(clip, index);
        emit mediaAdded(clip);
        // Success
        return true;
    }

    VOID_LOG_INFO("Invalid Media Type");
    return false;
}

void Project::Add(const SharedMediaClip& media)
{
    m_Media->Add(media);
    emit mediaAdded(media);
}

void Project::Add(const SharedPlaybackSequence& sequence)
{
    m_Media->Add(sequence);
    emit sequenceAdded(sequence);
}

void Project::Insert(const SharedMediaClip& media, const int index)
{
    m_Media->Insert(media, index);
    emit mediaAdded(media);
}

void Project::Insert(const SharedPlaybackSequence& sequence, const int index)
{
    m_Media->Insert(sequence, index);
    emit sequenceAdded(sequence);
}

bool Project::Remove(const QModelIndex& index)
{
    SharedMediaClip clip = m_Media->Media(index);
    if (clip)
    {
        emit mediaAboutToBeRemoved(clip);
        // QCoreApplication::processEvents();
        m_Media->Remove(index, false);
    
        return true;
    }
    return false;
}

Playlist* Project::NewPlaylist()
{
    std::string name = "Playlist ";
    name += std::to_string(m_Playlists->rowCount() + 1);
    return NewPlaylist(name);
}

Playlist* Project::NewPlaylist(const std::string& name)
{
    SetActivePlaylist(new Playlist(name, this));

    m_Playlists->Add(m_Playlist);
    emit playlistCreated(m_Playlist);

    return m_Playlist;
}

Playlist* Project::NewPlaylist(const std::string& name, int index)
{
    SetActivePlaylist(new Playlist(name, this));
    
    m_Playlists->Insert(m_Playlist, index);
    emit playlistCreated(m_Playlist);

    return m_Playlist;
}

void Project::SetCurrentPlaylist(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    SetActivePlaylist(m_Playlists->PlaylistAt(index));
    emit playlistChanged(m_Playlist);
}

void Project::SetCurrentPlaylist(int index)
{
    if (index < 0 || index > m_Playlists->rowCount() - 1)
        return;

    SetActivePlaylist(m_Playlists->PlaylistAt(index, 0));
    emit playlistChanged(m_Playlist);
}

void Project::RemovePlaylist(const QModelIndex& index)
{
    int row = index.row();
    m_Playlists->Remove(index);
    // Current playlist
    m_Playlist = nullptr;
    SetCurrentPlaylist(row >= m_Playlists->rowCount() ? [](int x) { return --x; }(m_Playlists->rowCount()) : row);
}

void Project::SetActivePlaylist(Playlist* playlist)
{
    if (!playlist)
        return;

    if (m_Playlist)
        m_Playlist->SetActive(false);

    m_Playlist = playlist;
    m_Playlist->SetActive(true);

    m_Playlists->Refresh();
}

SharedMediaClip Project::PlaylistMediaAt(const QModelIndex& index) const
{
    if (m_Playlist)
        return m_Playlist->Media(index);

    return nullptr;
}

SharedMediaClip Project::PlaylistMediaAt(int row, int column) const
{
    if (m_Playlist)
        return m_Playlist->Media(row, column);

    return nullptr;
}

void Project::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();
    out.AddMember("type", rapidjson::Value(TypeName(), allocator), allocator);

    /// Media
    rapidjson::Value clips(rapidjson::kArrayType);
    for (const SharedMediaClip& clip : m_Media->MediaClips())
    {
        rapidjson::Value clipObject;
        clip->Serialize(clipObject, allocator);
        clips.PushBack(clipObject, allocator);
    }

    out.AddMember("clips", clips, allocator);

    /// Sequences
    rapidjson::Value sequences(rapidjson::kArrayType);
    for (const SharedPlaybackSequence& sequence : m_Media->Sequences())
    {
        rapidjson::Value sequenceObject;
        sequence->Serialize(sequenceObject, allocator);
        sequences.PushBack(sequenceObject, allocator);
    }

    out.AddMember("sequences", sequences, allocator);

    /// Playlists
    rapidjson::Value playlists(rapidjson::kArrayType);
    for (const Playlist* playlist : *m_Playlists)
    {
        rapidjson::Value playlistObject;
        playlist->Serialize(playlistObject, allocator);
        playlists.PushBack(playlistObject, allocator);
    }

    out.AddMember("playlists", playlists, allocator);
}

void Project::Serialize(std::ostream& out) const
{
    uint32_t count = static_cast<uint32_t>(m_Media->MediaCount());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    // Serialize each of the clip data
    for (const SharedMediaClip& clip : m_Media->MediaClips())
        clip->Serialize(out);

    count = static_cast<uint32_t>(m_Media->SequenceCount());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const SharedPlaybackSequence& sequence : m_Media->Sequences())
        sequence->Serialize(out);

    count = static_cast<uint32_t>(m_Playlists->rowCount());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const Playlist* playlist : *m_Playlists)
        playlist->Serialize(out);
}

void Project::Deserialize(const rapidjson::Value& in)
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("Deserialize Project");

    /// Media
    const rapidjson::Value::ConstArray clips = in["clips"].GetArray();
    m_Media->ReserveMedia(clips.Size());
    for (unsigned int i = 0; i < clips.Size(); ++i)
    {
        SharedMediaClip clip = std::make_shared<MediaClip>(this);
        clip->Deserialize(clips[i]);
        m_Media->Add(clip);
    }

    /// Sequences
    const rapidjson::Value::ConstArray sequences = in["sequences"].GetArray();
    m_Media->ReserveSequences(sequences.Size());

    for (unsigned int i = 0; i < sequences.Size(); ++i)
    {
        SharedPlaybackSequence sequence = std::make_shared<PlaybackSequence>(this);
        sequence->Deserialize(sequences[i]);
        m_Media->Add(sequence);
    }

    /// Playlists
    const rapidjson::Value::ConstArray playlists = in["playlists"].GetArray();
    m_Playlists->Reserve(playlists.Size());
    for (unsigned int i = 0; i < static_cast<unsigned int>(playlists.Size()); ++i)
    {
        Playlist* playlist = new Playlist(this);
        playlist->Deserialize(playlists[i]);
        m_Playlists->Add(playlist);
    }
}

void Project::Deserialize(std::istream& in)
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("Deserialize Project");

    uint32_t count;

    /// Read Media
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    m_Media->ReserveMedia(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        SharedMediaClip clip = std::make_shared<MediaClip>(this);
        clip->Deserialize(in);
        m_Media->Add(clip);
    }

    /// Read Sequences
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    m_Media->ReserveSequences(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        SharedPlaybackSequence sequence = std::make_shared<PlaybackSequence>(this);
        sequence->Deserialize(in);
        m_Media->Add(sequence);
    }

    /// Read Playlists
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    m_Playlists->Reserve(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        Playlist* playlist = new Playlist(this);
        playlist->Deserialize(in);

        m_Playlists->Add(playlist);
    }
}

std::string Project::Document(const std::string& name) const
{
    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator = doc.GetAllocator();

    doc.AddMember("name", rapidjson::Value(name.c_str(), allocator), allocator);
    doc.AddMember("version", VOID_VERSION_NUMBER, allocator);

    rapidjson::Value project;
    Serialize(project, allocator);

    doc.AddMember("project", project, allocator);

    rapidjson::StringBuffer buffer;
    // Using Pretty Writer to dump with indented json formatting
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    doc.Accept(writer);
    return buffer.GetString();
}

void Project::ToStream(std::ostream& out, const std::string& name) const
{
    WriteString(out, name);

    int version = VOID_VERSION_NUMBER;
    out.write(reinterpret_cast<const char*>(&version), sizeof(version));

    Serialize(out);
}

Project* Project::FromDocument(const std::string& document)
{
    rapidjson::Document doc;
    doc.Parse(document.c_str());

    Project* p = new Project(doc["name"].GetString(), true);
    p->Deserialize(doc["clips"]);

    return p;
}

Project* Project::FromStream(std::istream& in)
{
    const std::string name = ReadString(in);

    int version;
    in.read(reinterpret_cast<char*>(&version), sizeof(version));

    Project* p = new Project(name, true);
    p->Deserialize(in);

    return p;
}

bool Project::Save()
{
    // The project hasn't been saved the first time yet, we don't know where the save path should be...
    if (m_Path.empty())
        return false;

    return SaveInternal(m_Path, m_Name, m_Type);
}

bool Project::Save(const std::string& path, const std::string& name, const EtherFormat::Type& type)
{
    return SaveInternal(path, name, type);
}

bool Project::SaveInternal(const std::string& path, const std::string& name, const EtherFormat::Type& type)
{
    if (type == EtherFormat::Type::ASCII)
        return SaveAscii(path, name);

    return SaveBinary(path, name);
}

bool Project::SaveAscii(const std::string& path, const std::string& name)
{
    std::ofstream out(path);
    if (!out.is_open())
    {
        VOID_LOG_ERROR("Not Able to save to Path: {0}", path);
        return false;
    }

    // Update internals
    m_Path = path;
    m_Name = name;
    m_Type = EtherFormat::Type::ASCII;

    // Serialize and onto the file
    out.write(EtherFormat::ASCII_MAGIC, EtherFormat::MAGIC_SIZE);
    out << '\n';
    out << Document(name);
    out.close();

    // The project is No longer considered modified
    m_Modified = false;
    return true;
}

bool Project::SaveBinary(const std::string& path, const std::string& name)
{
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open())
    {
        VOID_LOG_ERROR("Not Able to save to Path: {0}", path);
        return false;
    }

    // Update Internals
    m_Path = path;
    m_Name = name;
    m_Type = EtherFormat::Type::BINARY;

    out.write(EtherFormat::BINARY_MAGIC, EtherFormat::MAGIC_SIZE);
    // Now serialize all content on the binary
    ToStream(out, name);
    out.close();

    // The project is No longer considered modified
    m_Modified = false;
    return true;
}

} // namespace Core

VOID_NAMESPACE_CLOSE
