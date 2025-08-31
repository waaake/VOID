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
    , m_Name(name)
    , m_Path("")
    , m_Active(active)
    , m_Modified(false)
{
    m_Media = new MediaModel(this);
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
}

void Project::AddMedia(const SharedMediaClip& media)
{
    m_Media->Add(media);
    /* Update the modification state for the project */
    m_Modified = true;
}

void Project::InsertMedia(const SharedMediaClip& media, const int index)
{
    m_Media->Insert(media, index);
    /* Update the modification state for the project */
    m_Modified = true;
}

void Project::RemoveMedia(const QModelIndex& index)
{
    m_Media->Remove(index);
    /* Update the modification state for the project */
    m_Modified = true;
}

void Project::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();
    out.AddMember("type", rapidjson::Value(TypeName(), allocator), allocator);

    rapidjson::Value clips(rapidjson::kArrayType);

    for (const SharedMediaClip& clip : *m_Media)
    {
        rapidjson::Value clipObject;
        clip->Serialize(clipObject, allocator);
        clips.PushBack(clipObject, allocator);
    }

    out.AddMember("Clips", clips, allocator);
}

void Project::Deserialize(const rapidjson::Value& in)
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("Deserialize Project");

    const rapidjson::Value::ConstArray clips = in["Clips"].GetArray();
    VOID_LOG_INFO("Loading Clips: {0}", clips.Size());

    for (int i = 0; i < clips.Size(); ++i)
    {
        SharedMediaClip clip = std::make_shared<MediaClip>();
        clip->Deserialize(clips[i]);

        /* Add the Deserialized clip back */
        m_Media->Add(clip);
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

    doc.AddMember("Project", project, allocator);

    rapidjson::StringBuffer buffer;
    /* Using Pretty Writer to dump with indented json formatting */
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    doc.Accept(writer);
    return buffer.GetString();
}

Project* Project::FromDocument(const std::string& document)
{
    rapidjson::Document doc;
    doc.Parse(document.c_str());

    Project* p = new Project(doc["name"].GetString(), true);
    p->Deserialize(doc["clips"]);

    return p;
}

bool Project::Save()
{
    /* The project hasn't been saved yet */
    if (m_Path.empty())
        return false;

    return SaveInternal(m_Path, m_Name);
}

bool Project::SaveInternal(const std::string& path, const std::string& name)
{
    std::ofstream out(path);
    if (!out.is_open())
    {
        VOID_LOG_ERROR("Not Able to save to Path: {0}", path);
        return false;
    }

    /* Update internals */
    m_Path = path;
    m_Name = name;

    /* Serialize and onto the file */
    out << Document(name);
    out.close();

    /* The project is No longer modified */
    m_Modified = false;
    return true;
}

} // namespace Core

VOID_NAMESPACE_CLOSE
