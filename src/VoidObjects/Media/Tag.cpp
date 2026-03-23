// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Tag.h"

VOID_NAMESPACE_OPEN

Tag::Tag(const std::string& name)
    : m_Name(name)
{
}

Tag::~Tag()
{
}

void Tag::AddMetadata(const std::string& key, const std::string& value)
{
    m_Metadata[key] = value;
}

VOID_NAMESPACE_CLOSE
