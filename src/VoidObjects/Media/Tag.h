// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_TAG_H
#define _MEDIA_TAG_H

/* STD */
#include <string>
#include <unordered_map>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API Tag
{
public:
    Tag(const std::string& name);
    ~Tag();

    void SetName(const std::string& name) { m_Name = name; }
    void AddMetadata(const std::string& key, const std::string& value);
    
    const std::string& Name() const { return m_Name; }
    const std::unordered_map<std::string, std::string>& Metadata() const { return m_Metadata; }

private: /* Members */
    std::string m_Name;
    std::unordered_map<std::string, std::string> m_Metadata;
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_TAG_H
