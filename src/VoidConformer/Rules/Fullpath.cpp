// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Internal */
#include "Fullpath.h"

VOID_NAMESPACE_OPEN

bool FullpathRule::Match(
    const std::unordered_map<std::string, std::string>& sourceMedia,
    const std::unordered_map<std::string, std::string>& candidateMedia
) const
{
    return sourceMedia.at("path") == candidateMedia.at("path");
}

VOID_NAMESPACE_CLOSE
