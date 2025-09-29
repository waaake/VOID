// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MENU_SYSTEM_H
#define _VOID_MENU_SYSTEM_H

/* STD */
#include <string>
#include <functional>
#include <unordered_map>

/* Qt */
#include <QObject>
#include <QMenu>
#include <QAction>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/* Forward Decl for the Main Window (Imager) */
class VoidMainWindow;

class VOID_API MenuSystem : public QObject
{
    Q_OBJECT
public:
    explicit MenuSystem(VoidMainWindow* parent);
    QMenu* AddMenu(const std::string& name);
    QAction* AddAction(const std::string& menu, const std::string& action);
    void RegisterAction(const std::string& menu, const std::string& action, std::function<void()> callback, const std::string& shortcut = "");

private: /* Members */
    VoidMainWindow* m_Window;
    std::unordered_map<std::string, QMenu*> m_Menus;

private: /* Methods */
    QMenu* CreateMenu(const std::string& Name);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MENU_SYSTEM_H
