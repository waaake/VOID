// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Importer.h"
#include "Project.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"
#include "VoidUi/Commands/MediaCommands.h"

VOID_NAMESPACE_OPEN

DirectoryImporter::DirectoryImporter(Project* project, const std::string& directory, int maxLevel)
    : QRunnable()
    , m_Project(project)
    , m_Directory(directory)
    , m_MaxLevel(maxLevel)
{
}

void DirectoryImporter::run()
{
    const std::vector<MediaStruct> media = std::move(GetMedia(m_Directory));

    if (media.empty())
        return;

    m_Project->m_UndoStack->beginMacro("Import Directory");
    for (MediaStruct m : media)
    {
        m_Project->m_UndoStack->push(new MediaImportCommand(m.FirstPath()));
    }
    m_Project->m_UndoStack->endMacro();
}

std::vector<MediaStruct> DirectoryImporter::GetMedia(const std::string& directory, int level) const
{
    std::vector<MediaStruct> vec;

    Tools::VoidProfiler<std::chrono::duration<double>> p("Recursive Media Search");
    VOID_LOG_INFO("Searching Directory {0} at Level: {1}/{2}", directory, level, m_MaxLevel);

    try
    {
        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(directory))
        {
            /* Recurse through the directory if the level allows */
            if (entry.is_directory() && level <= m_MaxLevel)
            {
                /* Get all media inside the directory */
                std::vector<MediaStruct> out = std::move(GetMedia(entry.path().string(), level + 1));

                vec.reserve(vec.size() + out.size());
                vec.insert(vec.end(), std::make_move_iterator(out.begin()), std::make_move_iterator(out.end()));
            }

            MEntry e(entry.path().string());

            /* Flag to control what happens with the entry */
            bool new_entry = true;

            /**
             * Iterate over what we have in our vector currently
             * i.e. the media structs to see if this entry belongs to any one of them
             * if so, this gets added there, else we create a new media struct from it
             */
            for (MediaStruct& m : vec)
            {
                /**
                 * The entry belongs to this Media Struct don't have to add it again
                 * this search is going to be used to import media via the UndoQueue
                 * which only needs path of a single media from it
                 */
                if (m.Validate(e))
                {
                    new_entry = false;
                    break;
                }
            }

            /* Check if no entry in the MediaStruct adopted our newly created Media entry */
            if (new_entry)
            {
                vec.push_back(MediaStruct(e, MHelper::GetMediaType(e)));
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        VOID_LOG_ERROR(e.what());
    }

    return vec;
}

VOID_NAMESPACE_CLOSE
