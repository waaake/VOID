#ifndef _VOID_FORMAT_FORGE_H
#define _VOID_FORMAT_FORGE_H

/* STD */
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

/* Internal */
#include "Definition.h"
#include "ImageReader.h"

VOID_NAMESPACE_OPEN

/* Type defs */
using ImageForge = std::function<std::unique_ptr<ImageReader>()>;
// using MovieForge = std::function<std::unique_ptr<MovieReader>()>;

class Forge
{
public:
    /**
     * Returns the static instance of the Forge
     * allowing it to be singleton and accessors can then read from the same instance
     * where readers are registered
     */
    static Forge& Instance()
    {
        static Forge f;
        return f;
    }

    /**
     * Allows image Reader plugins for Formats to be registered
     * internally uses std::move to move the forger into the underlying register
     */
    void Register(const std::string& extension, ImageForge forger);

    /**
     * Returns a Registered ImageReader if found for the given extension
     * Else returns a null pointer instead
     */
    std::unique_ptr<ImageReader> GetImageReader(const std::string& extension) const;

    inline bool IsMovie(const std::string& extension) const { return false; }
    inline bool IsImage(const std::string& extension) const { return m_ImageForger.find(extension) != m_ImageForger.end(); }


private: /* Members */
    std::unordered_map<std::string, ImageForge> m_ImageForger;
    // std::unordered_map<std::string, MovieForge> m_MovieForger;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_FORMAT_FORGE_H
