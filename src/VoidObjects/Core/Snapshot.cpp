// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Snapshot.h"
#include "VoidCore/VoidTools.h"

VOID_NAMESPACE_OPEN

void Snapshot::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();
    out.AddMember("name", rapidjson::Value(name.c_str(), allocator), allocator);
    out.AddMember("description", rapidjson::Value(description.c_str(), allocator), allocator);
    out.AddMember(
        "data",
        rapidjson::Value(
            Tools::b64_encode(data).c_str(),
            allocator
        ),
        allocator
    );
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
    description = in["description"].GetString();
    data = Tools::b64_decode(in["data"].GetString());
}

void Snapshot::Deserialize(std::istream& in)
{
    name = ReadString(in);
    description = ReadString(in);
    data = ReadString(in);
}

VOID_NAMESPACE_CLOSE
