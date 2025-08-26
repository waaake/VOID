// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_DIRECTORY_IMPORTER_H
#define _VOID_MEDIA_DIRECTORY_IMPORTER_H

/* STD */
#include <vector>

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidCore/MediaFilesystem.h"

VOID_NAMESPACE_OPEN

class DirectoryImporter : public QObject
{
    Q_OBJECT

public:
    DirectoryImporter(const std::string& directory, int maxLevel = 5, QObject* parent = nullptr);
    ~DirectoryImporter();
    void Process();
    inline void Cancel() { m_Cancelled = true; }

signals:
    void maxCount(int);
    void progressUpdated(int);
    void mediaFound(const QString&);

    void startedImporting();
    void finishedImporting();

    void finished();

private: /* Members */
    std::string m_Directory;
    int m_MaxLevel;
    bool m_Cancelled;

private: /* Methods */
    std::vector<MediaStruct> GetMedia(const std::string& directory, int level = 0) const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_DIRECTORY_IMPORTER_H
