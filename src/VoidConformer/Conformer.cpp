// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Conformer.h"
#include "Commands/ConformCommands.h"
#include "VoidCore/Logging.h"
#include "VoidMediaPlayer/Project/Project.h"

VOID_NAMESPACE_OPEN

Conformer::Conformer(const std::vector<SharedMediaClip>& clips, const std::vector<SharedTrackItem>& items, Project* project)
    : m_Sources(clips)
    , m_Candidates(items)
    , m_Project(project)
{
}

std::unordered_set<SharedTrackItem> Conformer::Conform(const ConformOptions& options)
{
    std::unordered_set<SharedTrackItem> linked;
    linked.reserve(m_Candidates.size());

    QUndoStack* stack = m_Project->UndoStack();

    stack->beginMacro("Relink Media");
    for (const SharedMediaClip& source : m_Sources)
    {
        const std::unordered_map<std::string, std::string> sourceMetadata = Metadata(source);
        for (const SharedTrackItem& candidate : m_Candidates)
        {
            if (linked.find(candidate) != linked.end())
                continue;

            const std::unordered_map<std::string, std::string> candidateMetadata = Metadata(candidate);

            for (const ConformRule* rule : options.rules)
            {
                if (rule->Match(sourceMetadata, candidateMetadata))
                {
                    stack->push(new SetMediaCommand(source, candidate));
                    linked.insert(candidate);
                }
            }
        }
    }

    stack->endMacro();
    
    return linked;
}

std::unordered_map<std::string, std::string> Conformer::Metadata(const SharedMediaClip& media) const
{
    std::unordered_map<std::string, std::string> metadata;
    metadata["path"] = media->Path();

    return metadata;
}

std::unordered_map<std::string, std::string> Conformer::Metadata(const SharedTrackItem& item) const
{
    std::unordered_map<std::string, std::string> metadata;
    metadata["path"] = item->Path();

    return metadata;
}

VOID_NAMESPACE_CLOSE
