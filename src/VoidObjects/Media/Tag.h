// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_TAG_H
#define _MEDIA_TAG_H

/* STD */
#include <string>
#include <unordered_map>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Models/TagMetadataModel.h"

VOID_NAMESPACE_OPEN

class VOID_API Tag
{
public:
    explicit Tag(const std::string& name);
    Tag(const std::string& name, TagMetadataModel*& metadata);
    ~Tag();

    void SetName(const std::string& name) { m_Name = name; }
    
    const std::string& Name() const { return m_Name; }
    TagMetadataModel* MetadataModel() const { return m_Metadata; }

private: /* Members */
    std::string m_Name;
    TagMetadataModel* m_Metadata;
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_TAG_H
