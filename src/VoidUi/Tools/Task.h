// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TASK_WIDGET_H
#define _TASK_WIDGET_H

/* Qt */
#include <QLabel>
#include <QLayout>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Core/Task.h"

VOID_NAMESPACE_OPEN

class TaskWidget : public QWidget
{
    Q_OBJECT
private: /* Enums */
    enum class ActionState
    {
        Cancel,
        Retry
    };

public:
    TaskWidget(Task* task, QWidget* parent = nullptr);
    ~TaskWidget();

    TaskState State() const noexcept { return (m_Task) ? m_Task->State() : TaskState::Success; }
    bool Completed() const noexcept { return m_Task ? m_Task->Completed() : true; }
    void Close();

signals:
    void restartRequested(Task*);

protected:
    void paintEvent(QPaintEvent* event);

private: /* Members */
    QGridLayout* m_Layout;
    QLabel* m_TaskLabel;
    QLabel* m_StateLabel;
    QPushButton* m_LogButton;
    QPushButton* m_ActionButton;
    QProgressBar* m_ProgressBar;

    Task* m_Task;
    ActionState m_ActionState;

private: /* Methods */
    void Build();
    void Connect();
    void Setup();
    void UpdateState(const TaskState& state);
    void UpdateActionState(const ActionState& state);

    // Task Specific
    QString Label();
    void Cancel();
    void Restart();
    void Finished(const TaskState& state);
};

VOID_NAMESPACE_CLOSE

#endif // _TASK_WIDGET_H
