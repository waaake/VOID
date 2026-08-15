// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "DockManager.h"

VOID_NAMESPACE_OPEN

DockManager& DockManager::Instance()
{
    static DockManager d;
    return d;
}

DockManager::~DockManager()
{
}

int DockManager::RegisterDock(QWidget* widget, const std::string& text)
{
    int id = m_Docks.size();
    m_Docks[id] = {id, widget, text};
    widget->setObjectName(QString::number(id));
    emit updated();

    return id;
}

DockStruct DockManager::Dock(int index) const
{
    if (m_Docks.find(index) != m_Docks.end())
        return m_Docks.at(index);

    /* Invalid Dock Struct */
    return {-1, nullptr, ""};
}

VOID_NAMESPACE_CLOSE
