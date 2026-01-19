// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Internal */
#include "FormatForge.h"
#include "Media.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

Media::Media()
    : m_Framerate(24.0)
    , m_Type(Type::UNDEFINED)
{
}

Media::Media(const MediaStruct& mstruct)
    : Media()
{
    Read(mstruct);
}

Media::Media(MediaStruct& mstruct)
    : Media()
{
    Read(std::move(mstruct));
}

Media::~Media()
{
}

Media::Media(const std::string& basepath, const std::string& name, const std::string& extension)
    : Media()
{
    Read(MediaStruct(basepath, name, extension));
}

Media::Media(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding
    )
    : Media()
{
    Read(MediaStruct(basepath, name, extension, start, end, padding));
}

Media::Media(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding,
        const std::vector<v_frame_t>& missing
    )
    : Media()
{
    Read(MediaStruct(basepath, name, extension, start, end, padding, missing));
}

void Media::Read(const MediaStruct& mstruct)
{
    m_MediaStruct = mstruct;

    /**
     * Check if we have a Plugin reader for the file format
     * If not -> we can't proceed
     */
    if (!Forge::Instance().IsRegistered(m_MediaStruct.Extension()))
    {
        VOID_LOG_WARN("No Media Reader found for type {0}", m_MediaStruct.Extension());
        return;
    }

    m_MediaStruct.Process();
}

void Media::Read(MediaStruct&& mstruct)
{
    m_MediaStruct = std::move(mstruct);

    /**
     * Check if we have a Plugin reader for the file format
     * If not -> we can't proceed
     */
    if (!Forge::Instance().IsRegistered(m_MediaStruct.Extension()))
    {
        VOID_LOG_WARN("No Media Reader found for type {0}", m_MediaStruct.Extension());
        return;
    }

    m_MediaStruct.Process();
}

VOID_NAMESPACE_CLOSE
