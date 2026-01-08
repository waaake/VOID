// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYER_ENGINE_H
#define _VOID_PLAYER_ENGINE_H

/* Qt */
#include <QApplication>

/* Internal */
#include "Definition.h"
#include "VoidUi/BaseWindow/MenuSystem.h"
#include "VoidUi/BaseWindow/PlayerWindow.h"
#include "VoidApplication/Core/ArgumentParser.h"

VOID_NAMESPACE_OPEN

/**
 * Void Main Engine
 * - Initializes Logger
 * - Setup Application Defaults
 * - Initializes the Imager (MainWindow)
 * - Updates the Globals with the mainWindow Pointer allowing components to access elements
 * - Load Plugins
 */
class VoidEngine
{
public:
    // VoidEngine();
    ~VoidEngine();
    int Exec(int argc, char** argv);

private: /* Members */
    VoidMainWindow* m_Imager;
    MenuSystem* m_MenuSystem;
    ArgParser::EngineArguments m_Args;

private: /* Methods */
    void InitLogging();
    void Setup(QApplication& app);
    void Initialize();
    void PostInit();
    void PostStartup();
    void Callback();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_ENGINE_H
