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
using PixForge = std::function<std::unique_ptr<VoidPixReader>()>;

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
    void RegisterImageReader(const std::string& extension, PixForge forger);

    /**
     * Returns a Registered ImageReader if found for the given extension
     * Else returns a null pointer instead
     */
    std::unique_ptr<VoidPixReader> GetImageReader(const std::string& extension) const;
    std::unique_ptr<VoidPixReader> GetMovieReader(const std::string& extension) const;

    inline bool IsMovie(const std::string& extension) const { return false; }
    inline bool IsImage(const std::string& extension) const { return m_ImageForger.find(extension) != m_ImageForger.end(); }


private: /* Members */
    std::unordered_map<std::string, PixForge> m_ImageForger;
    std::unordered_map<std::string, PixForge> m_MovieForger;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_FORMAT_FORGE_H