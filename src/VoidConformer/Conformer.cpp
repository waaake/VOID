// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Conformer.h"
#include "Timecode.h"
#include "Commands/ConformCommands.h"
#include "VoidCore/Logging.h"
#include "VoidMediaPlayer/Project/Project.h"
#include "VoidConformer/Rules/Timecode.h"

VOID_NAMESPACE_OPEN

Conformer::Conformer(const std::vector<SharedMediaClip>& clips, const std::vector<SharedTrackItem>& items, Project* project)
    : m_Sources(clips)
    , m_Candidates(items)
    , m_Project(project)
{
}

std::unordered_set<SharedTrackItem> Conformer::Conform(const ConformOptions& options)
{
    TimecodeRule tcrule;

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

            /// Check for the timecode match first, if we can't match the timecode rule, we cannot proceed further
            /// This validates whether the source can be linked to the
            if (!tcrule.Match(sourceMetadata, candidateMetadata))
                continue;

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
    const MFrameRange fr = media->FrameRange();
    
    metadata["path"] = media->Path();
    metadata["start_frame"] = std::to_string(fr.startframe);
    metadata["end_frame"] = std::to_string(fr.endframe);
    metadata["duration"] = std::to_string(fr.duration);
    metadata["start_tc"] = Timecode::ToString(fr.startframe, fr.framerate);
    metadata["end_tc"] = Timecode::ToString(fr.endframe, fr.framerate);
    metadata["framerate"] = std::to_string(fr.framerate);

    return metadata;
}

const std::unordered_map<std::string, std::string>& Conformer::Metadata(const SharedTrackItem& item)
{
    if (m_MetadataStore.find(item) == m_MetadataStore.end())
    {
        std::unordered_map<std::string, std::string> metadata;
        
        metadata["path"] = item->Path();
        
        /// Timeline range
        const MFrameRange timefr = item->TimelineRange();
    
        metadata["timeline_in"] = std::to_string(timefr.startframe);
        metadata["timeline_out"] = std::to_string(timefr.endframe);
        metadata["timeline_duration"] = std::to_string(timefr.duration);
        metadata["timeline_in_tc"] = Timecode::ToString(timefr.startframe, timefr.framerate);
        metadata["timeline_out_tc"] = Timecode::ToString(timefr.endframe, timefr.framerate);
        metadata["timeline_framerate"] = std::to_string(timefr.framerate);
    
        /// Source range
        const MFrameRange sourcefr = item->SourceRange();
    
        metadata["source_in"] = std::to_string(sourcefr.startframe);
        metadata["source_out"] = std::to_string(sourcefr.endframe);
        metadata["source_duration"] = std::to_string(sourcefr.duration);
        metadata["source_in_tc"] = Timecode::ToString(sourcefr.startframe, sourcefr.framerate);
        metadata["source_out_tc"] = Timecode::ToString(sourcefr.endframe, sourcefr.framerate);
        metadata["source_framerate"] = std::to_string(sourcefr.framerate);

        m_MetadataStore[item] = metadata;
    }

    return m_MetadataStore[item];
}

VOID_NAMESPACE_CLOSE
