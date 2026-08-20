// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_TAG_H
#define _MEDIA_TAG_H

/* STD */
#include <string>
#include <unordered_map>

/* Internal */
#include "Definition.h"
#include "VoidObjects/VoidObject.h"
#include "VoidObjects/Models/TagMetadataModel.h"

VOID_NAMESPACE_OPEN

class VOID_API Tag : public VoidObject
{
public:
    explicit Tag(const std::string& name, QObject* parent = nullptr);
    Tag(const std::string& name, TagMetadataModel*& metadata, QObject* parent = nullptr);
    Tag(const std::string& name, const TagMetaStruct& metadata, QObject* parent = nullptr);
    ~Tag();

    void SetName(const std::string& name) { m_Name = name; }
    
    const std::string& Name() const { return m_Name; }
    const std::vector<std::pair<std::string, std::string>>& Metdata() const { return m_Metadata->Metadata(); }
    TagMetadataModel* MetadataModel() const { return m_Metadata; }

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Serialize(std::ostream& out) const override;
    
    void Deserialize(const rapidjson::Value& in) override;
    void Deserialize(std::istream& in) override;

    const char* TypeName() const { return "Tag"; }

private: /* Members */
    std::string m_Name;
    TagMetadataModel* m_Metadata;
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_TAG_H
