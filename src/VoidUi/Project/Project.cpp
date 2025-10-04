// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
// #include <QApplication>
#include <QThread>

/* Internal */
#include "Project.h"
#include "VoidUi/Commands/MediaCommands.h"

VOID_NAMESPACE_OPEN

Project::Project(const std::string& name, bool active, QObject* parent)
    : Core::Project(name, active, parent)
    , m_ProgressTask(nullptr)
    , m_DirectoryImporter(nullptr)
{
    m_UndoStack = new QUndoStack(this);
}

Project::Project(bool active, QObject* parent)
    : Project("", active, parent)
{
}

Project::~Project()
{
    m_UndoStack->deleteLater();

    DeleteProgressTask();
    DeleteImporter();
}

void Project::ImportDirectory(const std::string& directory, bool progressive)
{
    progressive ? ImportDirectoryP(directory) : ImportDirectory_(directory);
}

void Project::ImportDirectoryP(const std::string& directory)
{
    SetupProgressTask();

    QThread* thread = new QThread;
    m_DirectoryImporter = new DirectoryImporter(directory, 5);

    m_DirectoryImporter->moveToThread(thread);

    /* Thread */
    connect(thread, &QThread::started, m_DirectoryImporter, &DirectoryImporter::Process);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    /* Importer */
    connect(m_DirectoryImporter, &DirectoryImporter::startedImporting, this, [this]()
    {
        m_UndoStack->beginMacro("Import Directory");
        m_ProgressTask->SetTaskType("Importing...");
    });
    connect(m_DirectoryImporter, &DirectoryImporter::finishedImporting, this, [this]() { m_UndoStack->endMacro(); });

    connect(m_DirectoryImporter, &DirectoryImporter::finished, this, [this]()
    {
        DeleteProgressTask();
        DeleteImporter();
    });

    connect(m_DirectoryImporter, &DirectoryImporter::mediaFound, this, [this](const QString& path)
    {
        m_ProgressTask->SetCurrentTask(path.toStdString().c_str());
        m_UndoStack->push(new MediaImportCommand(path.toStdString()));
    });

    connect(m_DirectoryImporter, &DirectoryImporter::maxCount, m_ProgressTask, &ProgressTask::SetMaximum);
    connect(m_DirectoryImporter, &DirectoryImporter::progressUpdated, m_ProgressTask, &ProgressTask::SetValue);

    /* Start Import process */
    thread->start();
}


void Project::ImportDirectory_(const std::string& path)
{
    std::vector<MediaStruct> vec;

    try
    {
        for (std::filesystem::directory_entry entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (entry.is_regular_file())
            {
                MEntry e(entry.path().string());
    
                /* Flag to control what happens with the entry */
                bool new_entry = true;
    
                if (!e.SingleFile())
                {
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
                }
    
                /* Check if no entry in the MediaStruct adopted our newly created Media entry */
                if (new_entry)
                    vec.push_back(MediaStruct(e, MHelper::GetMediaType(e)));
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        VOID_LOG_ERROR(e.what());
    }

    m_UndoStack->beginMacro("Import Directory");

    /* Add All the found media */
    for (MediaStruct m : vec)
        m_UndoStack->push(new MediaImportCommand(m.FirstPath()));

    m_UndoStack->endMacro();
}

Project* Project::FromDocument(const std::string& document)
{
    rapidjson::Document doc;
    doc.Parse(document.c_str());

    Project* p = new Project(doc["name"].GetString(), true);
    p->Deserialize(doc["Project"]);

    return p;
}

Project* Project::FromStream(std::istream& in)
{
    const std::string name = ReadString(in);
    
    int version;
    in.read(reinterpret_cast<char*>(&version), sizeof(version));

    Project* p = new Project(name, true);
    p->Deserialize(in);

    return p;
}

void Project::SetupProgressTask()
{
    m_ProgressTask = new ProgressTask;
    m_ProgressTask->show();

    m_ProgressTask->SetTaskType("Searching...");
    m_ProgressTask->SetMaximum(0);

    connect(m_ProgressTask, &ProgressTask::cancelled, this, &Project::CancelImporting);
}

void Project::DeleteProgressTask()
{
    if (m_ProgressTask)
    {
        m_ProgressTask->deleteLater();
        delete m_ProgressTask;
        m_ProgressTask = nullptr;
    }
}

void Project::DeleteImporter()
{
    if (m_DirectoryImporter)
    {
        m_DirectoryImporter->deleteLater();
        delete m_DirectoryImporter;
        m_DirectoryImporter = nullptr;
    }
}

void Project::CancelImporting()
{
    if (m_DirectoryImporter)
        m_DirectoryImporter->Cancel();
}

Playlist* Project::NewPlaylist()
{
    /* Use default name */
    std::string name = "Playlist ";
    name += std::to_string(m_Playlists->rowCount() + 1);
    return NewPlaylist(name);
}

Playlist* Project::NewPlaylist(const std::string& name)
{
    SetActivePlaylist(new Playlist(name, this));

    /* Add to the playlists */
    m_Playlists->Add(m_Playlist);
    emit playlistCreated(m_Playlist);

    return m_Playlist;
}

Playlist* Project::NewPlaylist(const std::string& name, int index)
{
    SetActivePlaylist(new Playlist(name, this));
    
    m_Playlists->Insert(m_Playlist, index);
    emit playlistCreated(m_Playlist);

    return m_Playlist;
}

void Project::SetCurrentPlaylist(const QModelIndex& index)
{
    /* Provided index is not valid */
    if (!index.isValid())
        return;

    SetActivePlaylist(m_Playlists->PlaylistAt(index));
    emit playlistChanged(m_Playlist);
}

void Project::SetCurrentPlaylist(int index)
{
    if (index < 0 || index > m_Playlists->rowCount() - 1)
        return;

    SetActivePlaylist(m_Playlists->PlaylistAt(index, 0));
    emit playlistChanged(m_Playlist);
}

void Project::RemovePlaylist(const QModelIndex& index)
{
    int row = index.row();
    m_Playlists->Remove(index);

    /* Based on whether this is the last row or any from the beginning */
    SetCurrentPlaylist(row >= m_Playlists->rowCount() ? [](int x) { return --x; }(m_Playlists->rowCount()) : row);
}

void Project::SetActivePlaylist(Playlist* playlist)
{
    if (!playlist)
        return;

    /* Mark the current one as inactive */
    if (m_Playlist)
        m_Playlist->SetActive(false);

    m_Playlist = playlist;
    m_Playlist->SetActive(true);

    /* Force Update on the Model */
    m_Playlists->Refresh();
}

VOID_NAMESPACE_CLOSE
