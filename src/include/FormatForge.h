// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_FORMAT_FORGE_H
#define _VOID_FORMAT_FORGE_H

/* STD */
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

/* Internal */
#include "Definition.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

/* Type defs */
using PixForge = std::function<std::unique_ptr<VoidPixReader>(const std::string&, v_frame_t)>;
using MPixForge = std::function<std::unique_ptr<VoidMPixReader>(const std::string&, v_frame_t)>;

/**
 * Registry describing the Plugin
 * name
 * the media/file extensions it supports reading from
 * the Reader class
 */
template <typename Ty>
struct FormatRegistry
{
    /* Name of the plugin */
    std::string name;

    /* Media Extensions it allows to read */
    std::vector<std::string> extensions;

    /* The Image or Movie Reader to be registered */
    Ty reader;
};

class VOID_API Forge
{
    Forge() = default;

public:
    /**
     * Returns the static instance of the Forge
     * allowing it to be singleton and accessors can then read from the same instance
     * where readers are registered
     */
    static Forge& Instance();

    /* Disable Copy */
    Forge(const Forge&) = delete;
    Forge& operator=(const Forge&) = delete;

    /* Disable Move */
    Forge(Forge&&) = delete;
    Forge& operator=(Forge&&) = delete;

    /**
     * Allows image Reader plugins for Formats to be registered
     * internally uses std::move to move the forger into the underlying register
     */
    bool Register(const FormatRegistry<PixForge>& registry);
    bool Register(const FormatRegistry<MPixForge>& registry);

    /* Unregister all of the loaded plugins */
    void UnregisterPlugins();

    /**
     * Returns a Registered ImageReader if found for the given extension
     * Else returns a null pointer instead
     */
    std::unique_ptr<VoidPixReader> GetImageReader(const std::string& extension, const std::string& path, v_frame_t framenumber = 0) const;
    std::unique_ptr<VoidMPixReader> GetMovieReader(const std::string& extension, const std::string& path, v_frame_t framenumber = 0) const;

    std::unique_ptr<VoidPixReader> GetReader(const std::string& extension, const std::string& path, v_frame_t framenumber = 0) const;

    inline bool IsMovie(const std::string& extension) const { return m_MovieForger.find(extension) != m_MovieForger.end(); }
    inline bool IsImage(const std::string& extension) const { return m_ImageForger.find(extension) != m_ImageForger.end(); }

    bool IsRegistered(const std::string& extension) const;

private: /* Members */
    std::unordered_map<std::string, PixForge> m_ImageForger;
    std::unordered_map<std::string, MPixForge> m_MovieForger;

private: /* Methods */
    void RegisterImageReader(const std::string& extension, PixForge forger);
    void RegisterMovieReader(const std::string& extension, MPixForge forger);

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_FORMAT_FORGE_H
