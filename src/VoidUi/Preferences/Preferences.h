// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PREFERENCES_H
#define _VOID_PREFERENCES_H

/* Qt */
#include <QSettings>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

namespace Settings
{
    constexpr auto MissingFramesHandler = "player/missingFramesHandler";
    constexpr auto UndoQueueSize = "general/undoQueueSize";
    constexpr auto ColorStyle = "theme/colorStyle";
    constexpr auto MediaViewType = "mediaView/viewType";
    constexpr auto CacheMemory = "cache/memory";
    constexpr auto CacheThreads = "cache/threads";
    constexpr auto RecentProjects = "recents/projects";
    constexpr auto DontShowStartup = "startup/dontShowPopup";
    constexpr auto LastBrowsedLocation = "recents/browsed";
}

class VOID_API VoidPreferences : public QObject
{
    Q_OBJECT

public:
    static VoidPreferences& Instance();

    VoidPreferences(QObject* parent = nullptr);
    ~VoidPreferences();

    inline QVariant GetSetting(const std::string key) const { return m_Settings.value(key.c_str()); }
    inline void Set(const std::string& key, const QVariant& value)
    {
        /* Setup values */
        m_Settings.setValue(key.c_str(), value);
        /* and emit that something was changed if the hash changed */
        if (size_t h = Hash() != m_Hash)
        {
            emit updated();
            m_Hash = h;
        }
    }

    /* Helpers -> Exposing Setting Value natively */
    inline int GetMissingFrameHandler() const { return GetSetting(Settings::MissingFramesHandler).toInt(); }
    inline int GetUndoQueueSizeHint() const { return GetSetting(Settings::UndoQueueSize).toInt(); }
    inline int GetMediaViewType() const { return GetSetting(Settings::MediaViewType).toInt(); }
    inline unsigned long long GetCacheMemory() const { return GetSetting(Settings::CacheMemory).toULongLong(); }
    inline unsigned int GetCacheThreads() const { return GetSetting(Settings::CacheThreads).toUInt(); }
    inline int GetColorStyle() const { return GetSetting(Settings::ColorStyle).toInt(); }
    inline bool ShowStartup() const { return !GetSetting(Settings::DontShowStartup).toBool(); }
    inline QString LastBrowsed() const { return GetSetting(Settings::LastBrowsedLocation).toString(); }

    void AddRecentProject(const std::string& path);
    std::vector<std::string> RecentProjects();
    std::string MostRecentProject();

signals:
    /**
     * The updated signal is invoked when any setting is changed
     * this ensures any component listening to any values can get notified when something has changed
     * for it to requery it's data
     *
     * Adding individual signal for a setting could be quite messy in the long run
     */
    void updated();
    void projectsUpdated();

private: /* Members */
    QSettings m_Settings;
    size_t m_Hash;

private: /* Methods */
    void SaveRecentProjects(const std::vector<std::string>& files);
    size_t Hash();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PREFERENCES_H
