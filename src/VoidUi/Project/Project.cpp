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
    : Project("", active, parent)
{
}

Project::~Project()
{
    m_UndoStack->deleteLater();

    DeleteProgressTask();
    DeleteImporter();
}

void Project::ImportDirectory(const std::string& directory)
{
    SetupProgressTask();

    QThread* thread = new QThread;
    m_DirectoryImporter = new DirectoryImporter(directory, 5);

    m_DirectoryImporter->moveToThread(thread);

    /* Thread */
    connect(thread, &QThread::started, m_DirectoryImporter, &DirectoryImporter::Process);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    // connect(thread, &QThread::finished, this, &Project::DeleteProgressTask);

    /* Importer */
    connect(m_DirectoryImporter, &DirectoryImporter::started, this, [this]()
    {
        m_UndoStack->beginMacro("Import Directory");
        m_ProgressTask->SetTaskType("Importing");
    });

    connect(m_DirectoryImporter, &DirectoryImporter::finished, this, [this]()
    {
        m_UndoStack->endMacro();
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

void Project::SetupProgressTask()
{
    m_ProgressTask = new ProgressTask;
    m_ProgressTask->show();

    m_ProgressTask->SetTaskType("Searching");
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

VOID_NAMESPACE_CLOSE
