// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Internal */
#include "PartialName.h"

VOID_NAMESPACE_OPEN

bool PartialNameRule::Match(
    const std::unordered_map<std::string, std::string>& sourceMedia,
    const std::unordered_map<std::string, std::string>& candidateMedia
) const
{
    std::filesystem::path source(sourceMedia.at("path"));
    std::filesystem::path candidate(candidateMedia.at("path"));

    const std::string srcfile = source.filename().stem().string();
    const std::string candidfile = candidate.filename().stem().string();

    return (candidfile.find(srcfile) != std::string::npos) || (srcfile.find(candidfile) != std::string::npos);
}

VOID_NAMESPACE_CLOSE
