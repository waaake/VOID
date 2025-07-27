/* STD */
#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <string>
#include <sstream>
#include <filesystem>

/* Internal */
#include "FormatForge.h"
#include "Loader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/**
 * Windows uses ';' as the delimiting character
 * While linux and MacOS use ':'
 */
#if defined(_WIN32) || defined(__CYGWIN__)      // WINDOWS
const std::string DELIMITER = ";";
const std::string EXTENSION = ".dll";
#elif defined(__APPLE__)                        // APPLE
const std::string EXTENSION = ".dylib";
const std::string DELIMITER = ":";
#else                                           // Linux
const std::string EXTENSION = ".so";
const std::string DELIMITER = ":";
#endif

ReaderPluginLoader::ReaderPluginLoader()
{
}

ReaderPluginLoader::~ReaderPluginLoader()
{
    /* Unregister all of the Plugins */
    Forge::Instance().UnregisterPlugins();

    /* Unload all the plugins that were loaded */
    for (std::pair<std::string, Plugin>item : m_Loaded)
    {
        VOID_LOG_INFO("Unloading External plugin: {0}", item.first);
        Unload(item.second);
    }
}

void ReaderPluginLoader::LoadExternals()
{
    /* Populate the External Plugins from Environment variable */
    UpdatePluginPaths();

    /* Search Any plugins and Load all the Plugins Found */
    SearchAndLoadPlugins();
}

void ReaderPluginLoader::Load(const std::string& path)
{
    VOID_LOG_INFO("Loading External Plugin: {0}", path);

    /* Update on the Loaded map */
    Plugin p = LoadPlugin(path);

    /* If the plugin was loaded successfully */
    if (p.status)
    {
        m_Loaded[path] = p;
    }
    else
    {
        /* No point in keeping the plugin alive, free the memory */
        Unload(p);
    }
}

void ReaderPluginLoader::Unload(Plugin& plugin)
{
    /* Nothing for us to free */
    if (!plugin.handle)
        return;

    #if defined(_WIN32) || defined(__CYGWIN__)
    FreeLibrary(static_cast<HMODULE>(plugin.handle));
    #else
    dlclose(plugin.handle);
    #endif
}

Plugin ReaderPluginLoader::LoadPlugin(const std::string& path) const
{
    Plugin plug;
    /* Nothing loaded */
    plug.handle = nullptr;
    plug.status = false;

    #if (defined(_WIN32) || defined(__CYGWIN__))
    HMODULE handle = LoadLibrary(path.c_str());

    if (!handle)
        return plug;

    plug.handle = handle;
    /* Get the function pointer to the registerPlugin function */
    auto entrypoint = (void (*)(Forge&))GetProcAddress(handle, "registerPlugin");
    #else
    void* handle = dlopen(path.c_str(), RTLD_LAZY);

    plug.handle = handle;
    auto entrypoint = (void (*)(Forge&))dlsym(handle, "registerPlugin");
    #endif

    /* Have an entry point function to load? */
    if (entrypoint)
    {
        /* Register the Plugin through the Extern */
        entrypoint(Forge::Instance());
        plug.status = true;
    }

    return plug;
}

void ReaderPluginLoader::UpdatePluginPaths()
{
    /* Read the VOID_PLUGIN_PATH environment variable */
    const char* env_ = std::getenv("VOID_PLUGIN_PATH");

    /* If it has some contents -> try fetching all plugin paths */
    if (env_)
    {
        std::string paths = env_;
        std::stringstream ss(paths);
        std::string path;

        /* Use OS specific Delimiter */
        while (std::getline(ss, path, *(DELIMITER.c_str())))
        {
            if (!path.empty())
                m_PluginPaths.push_back(path);
        }
    }
}

void ReaderPluginLoader::SearchAndLoadPlugins()
{
    for (const std::string& path : m_PluginPaths)
    {
        /* Check if it's a valid directory and it exists */
        if (!(std::filesystem::exists(path) && std::filesystem::is_directory(path)))
            continue;

        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
        {
            /* A File? */
            if (entry.is_regular_file())
            {
                std::string ext = entry.path().extension().string();

                /* The file extension matches our desired EXTENSION */
                if (ext == EXTENSION)
                {
                    /* Load the Plugin*/
                    LoadPlugin(entry.path().string());
                }
            }
        }
    }
}

VOID_NAMESPACE_CLOSE
