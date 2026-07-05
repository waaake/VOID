// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include <thread>

/* Internal */
#include "Importer.h"
#include "Project.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

DirectoryImporter::DirectoryImporter(QObject* parent)
    : DirectoryImporter("", 5, parent)
{
}

DirectoryImporter::DirectoryImporter(const std::string& directory, int maxLevel, QObject* parent)
    : QObject(parent)
    , m_MaxLevel(maxLevel)
    , m_Cancelled(false)
{
    if (!directory.empty())
        m_Directories.push_back(directory);
}

DirectoryImporter::~DirectoryImporter()
{
    m_Cancelled.store(true);
    if (m_Worker.valid())
        m_Worker.get();
}

void DirectoryImporter::Import(const std::string& directory, int maxlevel)
{
    m_MaxLevel = maxlevel;
    m_Cancelled.store(false);
    m_Directories.push_back(directory);

    m_Worker = std::async(std::launch::async, &DirectoryImporter::Process, this);
}

void DirectoryImporter::Import(const std::vector<std::string>& directories, int maxlevel)
{
    m_MaxLevel = maxlevel;
    m_Cancelled.store(false);
    m_Directories = directories;

    m_Worker = std::async(std::launch::async, &DirectoryImporter::Process, this);
}

void DirectoryImporter::Process()
{
    std::vector<MediaStruct> media;
    for (auto& directory : m_Directories)
        GetMedia(directory, media);

    if (media.empty() || m_Cancelled.load())
    {
        emit finished();
        return;
    }

    emit startedImporting();
    /* Sets up the progress maximum count */
    emit maxCount(media.size());

    int count = 0;
    for (MediaStruct m : media)
    {
        if (m_Cancelled.load())
            break;

        emit progressUpdated(++count);
        emit mediaFound(QString::fromStdString(m.FirstPath()));

        // Add delay to allow user cancellations/interactions with the ui
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    emit finishedImporting();
    emit finished();
}

void DirectoryImporter::GetMedia(const std::string& directory, std::vector<MediaStruct>& media, int level) const
{
    try
    {
        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(directory))
        {

            if (m_Cancelled.load())
                return;

            /* Recurse through the directory if the level allows */
            if (entry.is_directory() && level <= m_MaxLevel)
            {
                GetMedia(entry.path().string(), media, level + 1);
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
            for (MediaStruct& m : media)
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
                media.push_back(MediaStruct(e, type));
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        VOID_LOG_ERROR(e.what());
    }
}

VOID_NAMESPACE_CLOSE
