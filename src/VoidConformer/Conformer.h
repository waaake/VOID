// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _CORE_CONFORMER_H
#define _CORE_CONFORMER_H

/* STD */
#include <vector>
#include <unordered_set>

/* Internal */
#include "Definition.h"
#include "ConformOptions.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

class Project;

class VOID_API Conformer
{
public:
    Conformer(const std::vector<SharedMediaClip>& clips, const std::vector<SharedTrackItem>& items, Project* project);
    std::unordered_set<SharedTrackItem> Conform(const ConformOptions& options);

private:
    std::vector<SharedMediaClip> m_Sources;
    std::vector<SharedTrackItem> m_Candidates;
    std::unordered_map<SharedTrackItem, std::unordered_map<std::string, std::string>> m_MetadataStore;
    Project* m_Project;

private:
    std::unordered_map<std::string, std::string> Metadata(const SharedMediaClip& media) const;
    const std::unordered_map<std::string, std::string>& Metadata(const SharedTrackItem& item);
};

VOID_NAMESPACE_CLOSE

#endif // _CORE_CONFORMER_H
