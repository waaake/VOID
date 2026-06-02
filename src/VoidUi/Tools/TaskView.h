// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TASK_VIEW_H
#define _TASK_VIEW_H

/* Qt */
#include <QWidget>
#include <QLayout>
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Core/Task.h"

VOID_NAMESPACE_OPEN

class TaskView : public QWidget
{
public:
    TaskView(QWidget* parent = nullptr);
    ~TaskView();

    // void AddTask();
    void AddTask(Task* task);
    void ClearCompleted();

private: /* Members */
    QVBoxLayout* m_Layout;
    QVBoxLayout* m_ScrollLayout;
    QPushButton* m_ClearButton;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
};

VOID_NAMESPACE_CLOSE

#endif // _TASK_VIEW_H
