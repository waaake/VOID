// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_DIRECTORY_IMPORTER_H
#define _VOID_MEDIA_DIRECTORY_IMPORTER_H

/* STD */
#include <atomic>
#include <future>
#include <vector>

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidCore/Media/Filesystem.h"

VOID_NAMESPACE_OPEN

class DirectoryImporter : public QObject
{
    Q_OBJECT

public:
    DirectoryImporter(QObject* parent = nullptr);
    DirectoryImporter(const std::string& directory, int maxLevel = 5, QObject* parent = nullptr);
    ~DirectoryImporter();
    void Import(const std::string& directory, int maxlevel = 5);
    inline void Cancel() { m_Cancelled.store(true); }

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
    std::atomic<bool> m_Cancelled;
    std::future<void> m_Worker;

private: /* Methods */
    void Process();
    std::vector<MediaStruct> GetMedia(const std::string& directory, int level = 0) const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_DIRECTORY_IMPORTER_H
