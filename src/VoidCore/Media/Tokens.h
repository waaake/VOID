// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_TOKENS_H
#define _MEDIA_TOKENS_H

/* STD */
#include <string>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct VOID_API ElementTokens
{
    std::string name;
    std::string version;
    int vnum { 0 };

    ElementTokens() {}
    ElementTokens(const std::string& name)
        : name(name) {}
    ElementTokens(std::string&& name, std::string&& version)
        : name(std::move(name)), version(std::move(version)) { ParseVersion(); }
    ElementTokens(const std::string& name, const std::string& version)
        : name(name), version(version) { ParseVersion(); }

    bool HasVersion() const noexcept { return !version.empty(); }
    bool Similar(const std::string& name) const noexcept { return name == this->name; }
    bool Similar(const ElementTokens& other) const noexcept { return other.name == name; }

    bool operator==(const ElementTokens& other) const noexcept { return other.name == name && other.vnum == vnum; }
    bool operator!=(const ElementTokens& other) const noexcept { return other.vnum != vnum || other.name != name; }
    bool operator>(const ElementTokens& other) const noexcept { return vnum > other.vnum; }
    bool operator<(const ElementTokens& other) const noexcept { return vnum < other.vnum; }

    explicit operator bool() const noexcept { return !name.empty(); }

private:
    void ParseVersion();
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_TOKENS_H
