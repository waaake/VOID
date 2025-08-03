// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLUGIN_LOADER_H
#define _VOID_PLUGIN_LOADER_H

/* STD */
#include <vector>
#include <unordered_map>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct Plugin
{
    void* handle;
    
    /* Holds if the registerPlugin symbol was loaded from the plugin */
    bool status;
};

class ReaderPluginLoader
{
    /* Private */
    ReaderPluginLoader();
public:
    static ReaderPluginLoader& Instance()
    {
        /* Single instance */
        static ReaderPluginLoader instance;

        /* Return the Loader */
        return instance;
    }

    /* Destructor */
    ~ReaderPluginLoader();

    /**
     * Loads External Plugins that have been specified with the VOID_PLUGINS_DIR environment variable
     */
    void LoadExternals();

    /* Loads plugin with the provided plugin path */
    void Load(const std::string& path);
    void Unload(Plugin& plugin);

private: /* Members */
    std::unordered_map<std::string, Plugin> m_Loaded;
    std::vector<std::string> m_PluginPaths;

private: /* Load Plugin */
    Plugin LoadPlugin(const std::string& path) const;

    /* Gets the environment variable value and splits to setup the plugin paths */
    void UpdatePluginPaths();
    /* Iterates over all the paths in the plugin paths, to try and find relevant plugins to be loaded */
    void SearchAndLoadPlugins();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLUGIN_LOADER_H
