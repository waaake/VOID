// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Controller.h"
#include "Globals.h"
#include "VoidBridge/Engine.h"

VOID_NAMESPACE_OPEN

EngineController::EngineController(QObject* parent)
    : QObject(parent)
{
    connect(&VoidEngineBridge::Instance(), &VoidEngineBridge::taskQueued, this, &EngineController::QueueTask);
}

void EngineController::QueueTask(Task* task)
{
    UIGlobals::QueueTask(task);
}

VOID_NAMESPACE_CLOSE
