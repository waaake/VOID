// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Internal */
#include "Timecode.h"
#include "FrameRange.h"

VOID_NAMESPACE_OPEN

bool TimecodeRule::Match(
    const std::unordered_map<std::string, std::string>& sourceMedia,
    const std::unordered_map<std::string, std::string>& candidateMedia
) const
{
    // return sourceMedia.at("path") == candidateMedia.at("path");
    // const MFrameRange mediafr(sourceMedia["start_frame"], sourceMedia["end_frame"], sourceMedia["duration"], sourceMedia["framerate"]);
    // const 
    const MFrameRange timelineSourceRange(
        std::stol(candidateMedia.at("source_in")),
        std::stol(candidateMedia.at("source_out")),
        std::stoi(candidateMedia.at("source_duration")),
        std::stod(candidateMedia.at("source_framerate"))
    );

    /// First check is to see whether the source media, has a starting frame that can work
    /// with the item's source's in and out
    /// e.g. the track item's source in is 967 and source out is 1010
    /// the source media we're trying to relink has to have a startframe lower or equal to 967
    /// else we can't relink to the item
    if (std::stol(sourceMedia.at("start_frame")) > timelineSourceRange.startframe)
        return false;

    /// This one can be considered a warning -- we need tri status around here...
    /// For now let's consider this failure
    if (std::stol(sourceMedia.at("end_frame")) < timelineSourceRange.endframe)
        return false;
    
    return true;
}

VOID_NAMESPACE_CLOSE
