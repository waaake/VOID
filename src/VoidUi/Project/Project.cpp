// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QThread>

/* Internal */
#include "Project.h"
#include "VoidUi/Commands/MediaCommands.h"

VOID_NAMESPACE_OPEN

Project::Project(const std::string& name, bool active, QObject* parent)
    : Core::Project(name, active, parent)
{
    m_UndoStack = new QUndoStack(this);
}

Project::Project(bool active, QObject* parent)
    : Core::Project(active, parent)
{
}

Project::~Project()
{
    m_UndoStack->deleteLater();
}

// void AddMedia(const std::string& directory)
// {
//     for (MediaStruct mstruct : MediaFS::FromDirectory(directory))
//     {

//     }
// }

void Project::ImportDirectory(const std::string& directory)
{
    // std::vector<MediaStruct> media = MediaFS().FromDirectory(directory);
    // std::vector<MediaStruct> media = MediaFS::GetAllMedia(directory);

    // if (media.empty())
    //     return;
    
    // m_UndoStack->beginMacro("Import Directory");
    // for (MediaStruct m : media)
    // {
    //     m_UndoStack->push(new MediaImportCommand(m.FirstPath()));
    // }
    // m_UndoStack->endMacro();

    QThread* thread = new QThread;
    DirectoryImporter* importer = new DirectoryImporter(directory, 5, this);

    /* Thread */
    connect(thread, &QThread::started, importer, &DirectoryImporter::process);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    /* Importer */
    connect(importer, &DirectoryImporter::started, this, [this]() { m_UndoStack->beginMacro("Import Directory"); });
    connect(importer, &DirectoryImporter::finished, this, [this](){ m_UndoStack->endMacro(); });
    connect(importer, &DirectoryImporter::finished, importer, &DirectoryImporter::deleteLater);
    connect(importer, &DirectoryImporter::mediaFound, this, [this](const std::string& path)
    {
        m_UndoStack->push(new MediaImportCommand(path)); 
    });

    thread->start();
}

VOID_NAMESPACE_CLOSE
