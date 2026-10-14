// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Internal */
#include "Filename.h"

VOID_NAMESPACE_OPEN

bool FilenameRule::Match(
    const std::unordered_map<std::string, std::string>& sourceMedia,
    const std::unordered_map<std::string, std::string>& candidateMedia
) const
{
    const std::filesystem::path source = sourceMedia.at("path");
    const std::filesystem::path candidate = candidateMedia.at("path");

    return source.filename() == candidate.filename();
}

VOID_NAMESPACE_CLOSE
