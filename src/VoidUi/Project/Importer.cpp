// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include <thread>

/* Internal */
#include "Importer.h"
#include "Project.h"

VOID_NAMESPACE_OPEN

DirectoryImporter::DirectoryImporter(const std::string& directory, int maxLevel, QObject* parent)
    : QObject(parent)
    , m_Directory(directory)
    , m_MaxLevel(maxLevel)
    , m_Cancelled(false)
{
}

DirectoryImporter::~DirectoryImporter()
{
}

void DirectoryImporter::Process()
{
    const std::vector<MediaStruct> media = std::move(GetMedia(m_Directory));

    if (media.empty() || m_Cancelled)
        return;

    emit started();
    emit maxCount(media.size());

    int count = 0;
    for (MediaStruct m : media)
    {
        if (m_Cancelled)
            break;

        emit progressUpdated(++count);
        emit mediaFound(QString::fromStdString(m.FirstPath()));

        /* Add delay to make it look like the media is imported as is being shown */
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    emit finished();
}

std::vector<MediaStruct> DirectoryImporter::GetMedia(const std::string& directory, int level) const
{
    std::vector<MediaStruct> vec;

    try
    {
        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(directory))
        {

            if (m_Cancelled)
                return vec;

            /* Recurse through the directory if the level allows */
            if (entry.is_directory() && level <= m_MaxLevel)
            {
                /* Get all media inside the directory */
                std::vector<MediaStruct> out = std::move(GetMedia(entry.path().string(), level + 1));

                vec.reserve(vec.size() + out.size());
                vec.insert(vec.end(), std::make_move_iterator(out.begin()), std::make_move_iterator(out.end()));

                continue;
            }

            MEntry e(entry.path().string());
            MediaType type = MHelper::GetMediaType(e);

            if (type == MediaType::NonMedia)
                continue;

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
                vec.push_back(MediaStruct(e, type));
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
