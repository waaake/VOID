// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QThreadPool>

/* Internal */
#include "Project.h"

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
    QThreadPool::globalInstance()->start(new DirectoryImporter(this, directory, 5));
}

VOID_NAMESPACE_CLOSE
