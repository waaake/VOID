// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MenuSystem.h"
#include "PlayerWindow.h"

VOID_NAMESPACE_OPEN

MenuSystem::MenuSystem(VoidMainWindow* parent)
    : QObject(parent)
    , m_Window(parent)
{
}

QMenu* MenuSystem::AddMenu(const std::string& name)
{
    if (m_Menus.find(name) != m_Menus.end())
        return m_Menus[name];
    return CreateMenu(name);
}

QAction* MenuSystem::AddAction(const std::string& menu, const std::string& action)
{
    QMenu* m = AddMenu(menu);
    QAction* a = new QAction(action.c_str(), m);
    m->addAction(a);

    return a;
}

void MenuSystem::RegisterAction(const std::string& menu, const std::string& action, std::function<void()> callback)
{
    QMenu* m = AddMenu(menu);
    QAction* a = new QAction(action.c_str(), m);
    m->addAction(a);

    connect(a, &QAction::triggered, this, [=]() { callback(); });
}

QMenu* MenuSystem::CreateMenu(const std::string& name)
{
    QMenu* menu = new QMenu(name.c_str(), m_Window);
    m_Window->MenuBar()->addMenu(menu);

    m_Menus[name] = menu;
    return menu;
}

VOID_NAMESPACE_CLOSE
