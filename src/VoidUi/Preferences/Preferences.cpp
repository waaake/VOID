// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Preferences.h"

VOID_NAMESPACE_OPEN

static int s_MaxRecentProjects = 5;

VoidPreferences::VoidPreferences(QObject* parent)
    : QObject(parent)
{
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
    QStringList recents = settings.value(Settings::RecentProjects).toStringList();

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

    settings.setValue(Settings::RecentProjects, recents);

    emit projectsUpdated();
}

VOID_NAMESPACE_CLOSE
