// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Threads.h"

VOID_NAMESPACE_OPEN

ThreadPool& ThreadPool::Instance()
{
    static ThreadPool instance;
    return instance;
}

ThreadPool::ThreadPool(QObject* parent)
    : QThreadPool(parent)
{
}

ThreadPool::~ThreadPool()
{
    Uninitialize();
}

void ThreadPool::Initialize()
{
    setMaxThreadCount(1);
}

void ThreadPool::Uninitialize()
{
    /* Clear any pending tasks from the queue */
    clear();
    /* Wait for the remaining to be done */
    waitForDone();
}

VOID_NAMESPACE_CLOSE
