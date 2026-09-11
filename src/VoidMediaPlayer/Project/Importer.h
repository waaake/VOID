// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_DIRECTORY_IMPORTER_H
#define _VOID_MEDIA_DIRECTORY_IMPORTER_H

/* STD */
#include <atomic>
#include <future>
#include <vector>
#include <unordered_set>

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidCore/Media/Filesystem.h"
#include "VoidObjects/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

enum class ImportType : uint8_t
{
    MEDIA,
    VERSIONS
};

struct ImportLog
{
    int max;
    int count;
    ImportType type;
    bool cancelled;

    ImportLog(int max, int count, const ImportType& type, bool cancelled = false)
        : max(max), count(count), type(type), cancelled(cancelled) {}
};

class DirectoryImporter : public QObject
{
    Q_OBJECT
public:
    DirectoryImporter(QObject* parent = nullptr);
    DirectoryImporter(const std::string& directory, int maxLevel = 5, QObject* parent = nullptr);
    ~DirectoryImporter();
    void Import(const std::string& directory, int maxlevel = 5);
    void Import(const std::vector<std::string>& directories, int maxlevel = 5);
    void ImportVersions(const std::vector<SharedMediaClip>& entries, int maxlevel = 2);
    inline void Cancel() { m_Cancelled.store(true); }

signals:
    void maxCount(int);
    void progressUpdated(int);
    void mediaFound(const QString&);

    void startedImporting();
    void finishedImporting(const ImportLog&);

    void finished();

private: /* Members */
    std::vector<std::string> m_Directories;
    std::vector<SharedMediaClip> m_Media;
    int m_MaxLevel;
    std::atomic<bool> m_Cancelled;
    std::future<void> m_Worker;

private: /* Methods */
    void Process();
    void ProcessVersions();
    void GetMedia(const std::string& directory, std::vector<MediaStruct>& media, int level = 0) const;
    std::vector<MediaStruct> GetMedia(const std::string& directory, int level = 0) const;
    std::vector<MediaStruct> GetVersionedMedia(
        const std::string& directory,
        const ElementTokens& entokens,
        const std::unordered_set<int>& existing,
        int level = 0
    ) const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_DIRECTORY_IMPORTER_H
