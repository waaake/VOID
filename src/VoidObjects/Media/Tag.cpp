// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Tag.h"

VOID_NAMESPACE_OPEN

Tag::Tag(const std::string& name, QObject* parent)
    : VoidObject(parent)
    , m_Name(name)
{
    m_Metadata = new TagMetadataModel;
}

Tag::Tag(const std::string& name, TagMetadataModel*& metadata, QObject* parent)
    : VoidObject(parent)
    , m_Name(name)
    , m_Metadata(metadata) // New owner for the metadata
{
    metadata = nullptr;
}

Tag::Tag(const std::string& name, const TagMetaStruct& metadata, QObject* parent)
    : VoidObject(parent)
    , m_Name(name)
    , m_Metadata(new TagMetadataModel(metadata))
{
}

Tag::~Tag()
{
    m_Metadata->deleteLater();
    delete m_Metadata;
    m_Metadata = nullptr;
}

void Tag::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();

    out.AddMember("type", rapidjson::Value(TypeName(), allocator), allocator);
    out.AddMember("name", rapidjson::Value(m_Name.c_str(), allocator), allocator);

    // Metadata
    rapidjson::Value metadata(rapidjson::kArrayType);
    for (const std::pair<std::string, std::string>& m : m_Metadata->Metadata())
    {
        rapidjson::Value entry(rapidjson::kObjectType);
        entry.AddMember("key", rapidjson::Value(m.first.c_str(), allocator), allocator);
        entry.AddMember("value", rapidjson::Value(m.second.c_str(), allocator), allocator);

        metadata.PushBack(entry, allocator);
    }

    out.AddMember("metadata", metadata, allocator);
}

void Tag::Serialize(std::ostream& out) const
{
    WriteString(out, m_Name);

    int count = static_cast<int>(m_Metadata->Metadata().size());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const std::pair<std::string, std::string>& m : m_Metadata->Metadata())
    {
        WriteString(out, m.first);
        WriteString(out, m.second);
    }
}

void Tag::Deserialize(const rapidjson::Value& in)
{
    m_Name = in["name"].GetString();

    const rapidjson::Value::ConstArray metadata = in["metadata"].GetArray();
    if (!metadata.Empty())
    {
        std::vector<std::pair<std::string, std::string>> m;
        m.reserve(metadata.Size());

        for (int i = 0; i < metadata.Size(); ++i)
            m.emplace_back(metadata[i]["key"].GetString(), metadata[i]["value"].GetString());

        m_Metadata->Set(std::move(m));
    }
}

void Tag::Deserialize(std::istream& in)
{
    m_Name = ReadString(in);
    int count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (count > 0)
    {
        std::vector<std::pair<std::string, std::string>> m;
        m.reserve(count);

        std::string k;
        std::string v;

        for (int i = 0; i < count; ++i)
        {            
            k = ReadString(in);
            v = ReadString(in);
            m.emplace_back(k, v);
        }

        m_Metadata->Set(std::move(m));
    }
}

VOID_NAMESPACE_CLOSE
