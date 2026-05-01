// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Tag.h"

VOID_NAMESPACE_OPEN

Tag::Tag(const std::string& name)
    : m_Name(name)
{
    m_Metadata = new TagMetadataModel;
}

Tag::Tag(const std::string& name, TagMetadataModel*& metadata)
    : m_Name(name)
    , m_Metadata(metadata) // New owner for the metadata
{
    metadata = nullptr;
}

Tag::Tag(const std::string& name, const TagMetaStruct& metadata)
    : m_Name(name)
    , m_Metadata(new TagMetadataModel(metadata))
{
}

Tag::~Tag()
{
    m_Metadata->deleteLater();
    delete m_Metadata;
    m_Metadata = nullptr;
}

VOID_NAMESPACE_CLOSE
