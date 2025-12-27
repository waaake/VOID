// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Preferences.h"

VOID_NAMESPACE_OPEN

static int s_MaxRecentProjects = 5;

VoidPreferences::VoidPreferences(QObject* parent)
    : QObject(parent)
{
    m_Hash = Hash();
}

VoidPreferences& VoidPreferences::Instance()
{
    static VoidPreferences instance;
    return instance;
}

VoidPreferences::~VoidPreferences()
{
}

void VoidPreferences::AddRecentProject(const std::string& path)
{
    std::vector<std::string> recents = RecentProjects();

    recents.erase(std::remove(recents.begin(), recents.end(), path), recents.end());
    recents.insert(recents.begin(), path);

    if (recents.size() > s_MaxRecentProjects)
        recents.resize(s_MaxRecentProjects);
    
    SaveRecentProjects(recents);
}

std::vector<std::string> VoidPreferences::RecentProjects()
{
    QStringList recents = m_Settings.value(Settings::RecentProjects).toStringList();

    std::vector<std::string> files;
    files.reserve(recents.size());

    for (const QString& recent : recents)
        files.emplace_back(recent.toStdString());
    
    return files;
}

std::string VoidPreferences::MostRecentProject()
{
    const std::vector<std::string>& recents = RecentProjects();

    if (recents.empty())
        return "";
    
    return recents.at(0);
}

void VoidPreferences::SaveRecentProjects(const std::vector<std::string>& files)
{
    QStringList recents;

    for (const std::string& file : files)
        recents << QString::fromStdString(file);

    m_Settings.setValue(Settings::RecentProjects, recents);

    emit projectsUpdated();
}

size_t VoidPreferences::Hash()
{
    /**
     * TODO: This is a bit expensive takes around ~0.003 seconds to process
     * Check on an alternative method or optimise this
     */
    size_t hash;

    size_t size = 0;
    std::string stringified;

    QStringList keys = m_Settings.allKeys();
    keys.sort();

    for (const QString& key : keys)
    {
        size += key.size();
        size += m_Settings.value(key).toString().size();
    }

    stringified.reserve(size);

    for (const QString& key : keys)
    {
        stringified += key.toStdString();
        stringified += m_Settings.value(key).toString().toStdString();
    }

    hash = std::hash<std::string>{}(stringified);
    return hash;
}

VOID_NAMESPACE_CLOSE
