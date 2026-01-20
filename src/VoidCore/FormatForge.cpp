// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "FormatForge.h"
#include "Logging.h"
#include "VoidTools.h"

VOID_NAMESPACE_OPEN

Forge& Forge::Instance()
{
    static Forge f;
    return f;
}

bool Forge::Register(const FormatRegistry<PixForge>& registry)
{
    for (const std::string& extension: registry.extensions)
    {
        RegisterImageReader(extension, registry.reader);
    }

    return true;
}

bool Forge::Register(const FormatRegistry<MPixForge>& registry)
{
    for (const std::string& extension: registry.extensions)
    {
        RegisterMovieReader(extension, registry.reader);
    }

    return true;
}

void Forge::UnregisterPlugins()
{
    /* Clear All of underlying structs */
    m_ImageForger.clear();
    m_MovieForger.clear();

    VOID_LOG_INFO("Unregistered All Reader Plugins.");
}

void Forge::RegisterImageReader(const std::string& extension, PixForge forger)
{
    /* Show a warning if the plugin was already registered */
    if (m_ImageForger.find(extension) != m_ImageForger.end())
    {
        VOID_LOG_WARN("Image reader for {0} extension already registered. Ignoring registration.");
        return;
    }

    VOID_LOG_INFO("Registering Image Reader for extension: {0}", extension);

    /* Move the forger to the registry */
    m_ImageForger[extension] = std::move(forger);
}

void Forge::RegisterMovieReader(const std::string& extension, MPixForge forger)
{
    /* Show a warning if the plugin was already registered */
    if (m_MovieForger.find(extension) != m_MovieForger.end())
    {
        VOID_LOG_WARN("Movie reader for {0} extension already registered. Ignoring registration.");
        return;
    }

    VOID_LOG_INFO("Registering Movie Reader for extension: {0}", extension);

    /* Move the forger to the registry */
    m_MovieForger[extension] = std::move(forger);
}

std::unique_ptr<VoidPixReader> Forge::GetImageReader(const std::string& extension, const std::string& path, v_frame_t framenumber) const
{
    std::unordered_map<std::string, PixForge>::const_iterator it = m_ImageForger.find(extension);
    return it == m_ImageForger.end() ? nullptr : it->second(path, framenumber);
}

std::unique_ptr<VoidMPixReader> Forge::GetMovieReader(const std::string& extension, const std::string& path, v_frame_t framenumber) const
{
    std::unordered_map<std::string, MPixForge>::const_iterator it = m_MovieForger.find(extension);
    return it == m_MovieForger.end() ? nullptr : it->second(path, framenumber);
}

std::unique_ptr<VoidPixReader> Forge::GetReader(const std::string& extension, const std::string& path, v_frame_t framenumber) const
{
    auto it = m_ImageForger.find(extension);
    
    if (it == m_ImageForger.end())
    {
        auto mIt = m_MovieForger.find(extension);
        return mIt == m_MovieForger.end() ? nullptr : mIt->second(path, framenumber);
    }

    return it->second(path, framenumber);
}

bool Forge::IsRegistered(const std::string& extension) const
{
    return m_ImageForger.find(extension) != m_ImageForger.end() || m_MovieForger.find(extension) != m_MovieForger.end();
}

VOID_NAMESPACE_CLOSE
