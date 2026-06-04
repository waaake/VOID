// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _INTERNAL_TASK_PROCESS_H
#define _INTERNAL_TASK_PROCESS_H

/* STD */
#include <atomic>

/* Qt */
#include <QObject>
#include <QRunnable>

/* Internal */
#include "Definition.h"
#include "TaskLog.h"

VOID_NAMESPACE_OPEN

enum class TaskState
{
    Queued,
    Running,
    Cancelling,
    Cancelled,
    Failed,
    Success
};

class VOID_API Task : public QObject, public QRunnable
{
    Q_OBJECT
public:
    Task(const std::string& name);
    virtual ~Task();

    void run() override;

    inline TaskLogModel* Logs() { return m_Logs; }

    void Cancel() noexcept { m_Cancelled.store(true); }
    inline bool Cancelled() const noexcept { return m_Cancelled.load(); }
    
    inline const std::string& Name() const noexcept { return m_Name; }
    inline virtual std::string Label() const { return ""; }

    inline const TaskState State() const noexcept { return m_State; }
    inline bool Started() const noexcept { return m_State != TaskState::Queued; }
    inline bool Completed() const noexcept { return m_State == TaskState::Success; }

signals:
    void stateChanged(const TaskState&);
    void finished(const TaskState&);
    void maxUpdated(int);
    void progressed(int steps);

protected:
    inline void SetMax(int max) { emit maxUpdated(max); }
    inline void SetProgress(int steps) { emit progressed(steps); }
    inline void SetState(const TaskState& state) { m_State = state; emit stateChanged(state); }
    inline void Finish(const TaskState& state) { m_State = state; emit finished(state); }
    inline void Log(const QString& text, const TaskLog::Level& level) { m_Logs->AddLog(text, level); }

    // The actual work happens here
    // run method above just takes care of the statuses and starting up the work
    // Any exceptions needs to be handled explicitly by the child
    virtual bool Work() = 0;

private:
    std::atomic_bool m_Cancelled;
    TaskState m_State;
    TaskLogModel* m_Logs;
    std::string m_Name;
};

VOID_NAMESPACE_CLOSE

#endif // _INTERNAL_TASK_PROCESS_H
