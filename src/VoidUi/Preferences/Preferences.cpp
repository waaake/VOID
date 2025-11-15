// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Preferences.h"

VOID_NAMESPACE_OPEN

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

void VoidPreferences::AddRecentProject(const RecentProjects& index, const std::string& path)
{
    settings.setValue(QString::number(static_cast<int>(index)), QVariant(path.c_str()));
    emit projectsUpdated();
}

std::string VoidPreferences::GetRecentProject(const RecentProjects& index)
{
    return settings.value(QString::number(static_cast<int>(index))).toString().toStdString();
}

VOID_NAMESPACE_CLOSE
