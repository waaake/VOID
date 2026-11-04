// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _ENGINE_CONTROLLER_H
#define _ENGINE_CONTROLLER_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Core/Task.h"

VOID_NAMESPACE_OPEN

class VOID_API EngineController : public QObject
{
public:
    EngineController(QObject* parent = nullptr);
    void QueueTask(Task* task);
};

VOID_NAMESPACE_CLOSE

#endif // _ENGINE_CONTROLLER_H
