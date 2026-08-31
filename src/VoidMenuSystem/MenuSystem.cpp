// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MenuSystem.h"

VOID_NAMESPACE_OPEN

MenuSystem::MenuSystem(QMenuBar* menubar)
    : QObject(menubar)
    , m_Menubar(menubar)
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

QAction* MenuSystem::AddAction(QMenu* menu, const std::string& action)
{
    QAction* a = new QAction(action.c_str(), menu);
    menu->addAction(a);

    return a;
}

QAction* MenuSystem::AddAction(QMenu* menu, const std::string& action, const QKeySequence& shortcut)
{
    QAction* a = new QAction(action.c_str(), menu);
    menu->addAction(a);
    a->setShortcut(shortcut);

    return a;
}

void MenuSystem::RegisterAction(const std::string& menu, const std::string& action, std::function<void()> callback, const std::string& shortcut)
{
    QMenu* m = AddMenu(menu);
    QAction* a = new QAction(action.c_str(), m);
    m->addAction(a);

    if (!shortcut.empty())
        a->setShortcut(QKeySequence(shortcut.c_str()));

    connect(a, &QAction::triggered, this, [=]() { callback(); });
}

bool MenuSystem::RegisterContextMenu(QMenu* menu, const std::string& name)
{
    if (m_ContextMenu.find(name) == m_ContextMenu.end())
    {
        m_ContextMenu[name] = menu;
        return true;
    }

    return false;
}

QMenu* MenuSystem::RegisterContextMenu(const std::string& name)
{
    if (m_ContextMenu.find(name) == m_ContextMenu.end())
    {
        QMenu* menu = new QMenu();
        m_ContextMenu[name] = menu;
        return menu;
    }

    return m_ContextMenu[name];
}

QMenu* MenuSystem::ContextMenu(const std::string& name) const
{
    auto it = m_ContextMenu.find(name);
    return it == m_ContextMenu.end() ? nullptr : it->second;
}

QAction* MenuSystem::AddContextMenuAction(const std::string& menu, const std::string& action, std::function<void()> callback)
{
    if (QMenu* m = ContextMenu(menu))
    {
        QAction* a = new QAction(action.c_str(), m);
        m->addAction(a);

        connect(a, &QAction::triggered, this, [=]() { callback(); });

        return a;
    }

    return nullptr;
}

QMenu* MenuSystem::CreateMenu(const std::string& name)
{
    QMenu* menu = new QMenu(name.c_str(), m_Menubar);
    m_Menubar->addMenu(menu);

    m_Menus[name] = menu;
    return menu;
}

VOID_NAMESPACE_CLOSE
