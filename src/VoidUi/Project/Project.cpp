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
    DeleteProgressTask();
}

// void AddMedia(const std::string& directory)
// {
//     for (MediaStruct mstruct : MediaFS::FromDirectory(directory))
//     {

//     }
// }

void Project::ImportDirectory(const std::string& directory)
{
    SetupProgressTask();

    QThread* thread = new QThread;
    DirectoryImporter* importer = new DirectoryImporter(directory, 5);

    importer->moveToThread(thread);

    /* Thread */
    connect(thread, &QThread::started, importer, &DirectoryImporter::process);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    /* Importer */
    connect(importer, &DirectoryImporter::started, this, [this]()
    {
        m_UndoStack->beginMacro("Import Directory");
    });

    connect(importer, &DirectoryImporter::finished, this, [this]()
    {
        m_UndoStack->endMacro();
        DeleteProgressTask();
    });

    connect(importer, &DirectoryImporter::finished, importer, &DirectoryImporter::deleteLater);
    connect(importer, &DirectoryImporter::mediaFound, this, [this](const QString& path)
    {
        m_ProgressTask->SetCurrentTask(path.toStdString().c_str());
        m_UndoStack->push(new MediaImportCommand(path.toStdString()));
    });
    connect(importer, &DirectoryImporter::maxCount, m_ProgressTask, &ProgressTask::SetMaximum);
    connect(importer, &DirectoryImporter::progressUpdated, m_ProgressTask, &ProgressTask::SetValue);

    thread->start();
}

void Project::SetupProgressTask()
{
    m_ProgressTask = new ProgressTask;
    m_ProgressTask->show();

    m_ProgressTask->SetTaskType("Searching");
    m_ProgressTask->SetMaximum(0);
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

VOID_NAMESPACE_CLOSE
