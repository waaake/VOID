/* Internal */
#include "FormatForge.h"
#include "Logging.h"

VOID_NAMESPACE_OPEN

void Forge::Register(const std::string& extension, ImageForge forger)
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

std::unique_ptr<ImageReader> Forge::GetImageReader(const std::string& extension) const
{
    std::unordered_map<std::string, ImageForge>::const_iterator it = m_ImageForger.find(extension);
    /* If the extension is available */
    if (it != m_ImageForger.end())
        return it->second();
    
    /* The extension has not yet been registered */
    return nullptr;
}

VOID_NAMESPACE_CLOSE
