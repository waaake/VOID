// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_DIRECTORY_IMPORTER_H
#define _VOID_MEDIA_DIRECTORY_IMPORTER_H

/* STD */
#include <vector>

/* Qt */
#include <QRunnable>

/* Internal */
#include "Definition.h"
#include "VoidCore/MediaFilesystem.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration of Project */
class Project;

class DirectoryImporter : public QRunnable
{
public:
    DirectoryImporter(Project* project, const std::string& directory, int maxLevel = 5);
    void run() override;

private: /* Members */
    Project* m_Project;
    std::string m_Directory;
    int m_MaxLevel;

private: /* Methods */
    std::vector<MediaStruct> GetMedia(const std::string& directory, int level = 0) const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_DIRECTORY_IMPORTER_H
