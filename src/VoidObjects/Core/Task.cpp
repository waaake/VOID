// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Task.h"

VOID_NAMESPACE_OPEN

Task::Task(const std::string& name)
    : m_Cancelled(false)
    , m_State(TaskState::Queued)
    , m_Name(name)
{
    setAutoDelete(false);
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
