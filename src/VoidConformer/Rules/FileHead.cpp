// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Internal */
#include "FileHead.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

bool FileHeadRule::Match(
    const std::unordered_map<std::string, std::string>& sourceMedia,
    const std::unordered_map<std::string, std::string>& candidateMedia
) const
{
    std::filesystem::path source(sourceMedia.at("path"));
    std::filesystem::path candidate(candidateMedia.at("path"));

    return source.filename().stem() == candidate.filename().stem();
}

VOID_NAMESPACE_CLOSE
