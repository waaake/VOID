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

    // Need to have a better way to deal with the return status or not have this at all
    return true;
}

bool Forge::Register(const IOpRegistry& registry)
{
    VOID_LOG_INFO("Registering Image Operator: {0}", registry.name);
    auto it = m_IOpForger.emplace(registry.name, std::move(registry.iop));
    return it.second;
}

bool Forge::Register(const WriterRegistry& registry)
{
    if (registry.type == WriterType::Image)
    {
        for (const auto& extension : registry.extensions)
            RegisterImageWriter(extension, registry.writer);
    }
    else
    {
        for (const auto& extension : registry.extensions)
            RegisterMovieWriter(extension, registry.writer);
    }

    return true;
}

void Forge::UnregisterPlugins()
{
    /* Clear All of underlying structs */
    m_ImageForger.clear();
    m_MovieForger.clear();
    m_IOpForger.clear();

    VOID_LOG_INFO("Unregistered All Plugins.");
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

void Forge::RegisterImageWriter(const std::string& extension, PixWriterForge forger)
{
    if (m_ImageWriters.find(extension) != m_ImageWriters.end())
    {
        VOID_LOG_WARN("Image writer for {0} extension is already registered. Ignoring registration");
        return;
    }

    VOID_LOG_INFO("Registering Image Writer for extension: {0}", extension);
    m_ImageWriters[extension] = std::move(forger);
}

void Forge::RegisterMovieWriter(const std::string& extension, PixWriterForge forger)
{
    if (m_MovieWriters.find(extension) != m_MovieWriters.end())
    {
        VOID_LOG_WARN("Movie writer for {0} extension is already registered. Ignoring registration");
        return;
    }

    VOID_LOG_INFO("Registering Movie Writer for extension: {0}", extension);
    m_MovieWriters[extension] = std::move(forger);
}

std::unique_ptr<VoidPixReader> Forge::GetImageReader(const std::string& extension, const std::string& path, v_frame_t framenumber) const
{
    std::unordered_map<std::string, PixForge>::const_iterator it = m_ImageForger.find(extension);
    return it == m_ImageForger.end() ? nullptr : it->second(path, framenumber);
}

std::unique_ptr<VoidPixReader> Forge::ImageReaderAt(const std::string& extension, const std::string& path, v_frame_t framenumber) const
{
    return m_ImageForger.at(extension)(path, framenumber);
}

std::unique_ptr<VoidMPixReader> Forge::GetMovieReader(const std::string& extension, const std::string& path, v_frame_t framenumber) const
{
    std::unordered_map<std::string, MPixForge>::const_iterator it = m_MovieForger.find(extension);
    return it == m_MovieForger.end() ? nullptr : it->second(path, framenumber);
}

std::unique_ptr<VoidMPixReader> Forge::MovieReaderAt(const std::string& extension, const std::string& path, v_frame_t framenumber) const
{
    return m_MovieForger.at(extension)(path, framenumber);
}

std::unique_ptr<ImageOp> Forge::GetImageOp(const std::string& name) const
{
    std::unordered_map<std::string, IOpForge>::const_iterator it = m_IOpForger.find(name);
    return it == m_IOpForger.end() ? nullptr : it->second();
}

std::unique_ptr<PixWriter> Forge::GetImageWriter(const std::string& extension, const EncodeSpec& spec) const
{
    auto it = m_ImageWriters.find(extension);
    return it == m_ImageWriters.end() ? nullptr : it->second(spec);
}

std::unique_ptr<PixWriter> Forge::GetMovieWriter(const std::string& extension, const EncodeSpec& spec) const
{
    auto it = m_MovieWriters.find(extension);
    return it == m_MovieWriters.end() ? nullptr : it->second(spec);
}

bool Forge::IsRegistered(const std::string& extension) const
{
    return m_ImageForger.find(extension) != m_ImageForger.end() || m_MovieForger.find(extension) != m_MovieForger.end();
}

VOID_NAMESPACE_CLOSE
