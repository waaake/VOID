// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Snapshot.h"

VOID_NAMESPACE_OPEN

void Snapshot::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();
    out.AddMember("name", rapidjson::Value(name.c_str(), allocator), allocator);
    out.AddMember("description", rapidjson::Value(description.c_str(), allocator), allocator);
    out.AddMember("data", rapidjson::Value(data.c_str(), allocator), allocator);
}

void Snapshot::Serialize(std::ostream& out) const
{
    WriteString(out, name);
    WriteString(out, description);
    WriteString(out, data);
}

void Snapshot::Deserialize(const rapidjson::Value& in)
{
    name = in["name"].GetString();
    description = in["name"].GetString();
    data = in["name"].GetString();
}

void Snapshot::Deserialize(std::istream& in)
{
    ReadString(in, name);
    ReadString(in, description);
    ReadString(in, data);
}

VOID_NAMESPACE_CLOSE
