// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Task.h"

VOID_NAMESPACE_OPEN

Task::Task(const std::string& name)
    : QObject()
    , m_Cancelled(false)
    , m_State(TaskState::Queued)
    , m_Name(name)
{
    setAutoDelete(false);
    m_Logs = new TaskLogModel(this);
}

Task::~Task()
{
    m_Logs->deleteLater();
    delete m_Logs;
    m_Logs = nullptr;
}

void Task::run()
{
    // Was cancelled before starting
    if (m_Cancelled.load())
        return;

    SetState(TaskState::Running);
    bool status = Work();
    Finish(m_Cancelled.load() ? TaskState::Cancelled : (status) ? TaskState::Success : TaskState::Failed);

    // Reset the cancel status, in case we want to restart the task later on if it's cancelled
    m_Cancelled.store(false);
}

VOID_NAMESPACE_CLOSE
