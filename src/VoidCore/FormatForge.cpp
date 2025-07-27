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

std::unique_ptr<VoidPixReader> Forge::GetImageReader(const std::string& extension) const
{
    std::unordered_map<std::string, PixForge>::const_iterator it = m_ImageForger.find(extension);
    /* If the extension is available */
    if (it != m_ImageForger.end())
        return it->second();

    /* The extension has not yet been registered */
    return nullptr;
}

std::unique_ptr<VoidMPixReader> Forge::GetMovieReader(const std::string& extension) const
{
    std::unordered_map<std::string, MPixForge>::const_iterator it = m_MovieForger.find(extension);
    /* If the extension is available */
    if (it != m_MovieForger.end())
        return it->second();

    /* The extension has not yet been registered */
    return nullptr;
}

VOID_NAMESPACE_CLOSE
