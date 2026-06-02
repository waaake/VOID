// Copyright (c) 2025 waaake
// Licensed under the MIT License

// #include <thread>

/* Qt */
#include <QPainter>
#include <QScrollArea>

/* Internal */
#include "Task.h"
#include "TaskView.h"
#include "VoidObjects/Core/Threads.h"
// #include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/// Temporary Dummy Task

// class DummyTask : public Task
// {
// public:
//     DummyTask() : Task("Dummy Release") {}
//     ~DummyTask()
//     {
//         VOID_LOG_INFO("Dummy Release task destroyed");
//     }

// protected:
//     bool Work() override
//     {
//         SetMax(20);

//         for (int i = 0; i < 20; ++i)
//         {
//             std::this_thread::sleep_for(std::chrono::milliseconds(10));
//             VOID_LOG_INFO("Iter::{0}", i);
//             if (Cancelled())
//                 return false;

//             SetProgress(i);
//         }

//         return false;
//     }
// };


/// Base Widget

class BaseWidget : public QWidget
{
protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), palette().color(QPalette::Base));
    }
};


/// Task View

TaskView::TaskView(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Setup();
    Connect();
}

TaskView::~TaskView()
{
    m_ScrollLayout->deleteLater();
    delete m_ScrollLayout;
    m_ScrollLayout = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

// void TaskView::AddTask()
// {
//     DummyTask* task = new DummyTask;
//     // Task is now owned by the TaskWidget and should no longer be touched outside
//     TaskWidget* taskwid = new TaskWidget(task, this);
//     m_ScrollLayout->addWidget(taskwid);

//     ThreadPool::Instance().start(task);
//     connect(taskwid, &TaskWidget::restartRequested, this, [this](Task* task) -> void
//     {
//         ThreadPool::Instance().start(task);
//     });
// }

void TaskView::AddTask(Task* task)
{
    // Task is now owned by the TaskWidget and should no longer be touched outside
    TaskWidget* taskwid = new TaskWidget(task, this);
    m_ScrollLayout->addWidget(taskwid);

    ThreadPool::Instance().start(task);
    connect(taskwid, &TaskWidget::restartRequested, this, [this](Task* task) -> void
    {
        ThreadPool::Instance().start(task);
    });
}

void TaskView::ClearCompleted()
{
    for (auto& task : findChildren<TaskWidget*>())
    {
        if (task->Completed())
            task->Close();
    }
}

void TaskView::Build()
{
    m_Layout = new QVBoxLayout(this);

    QHBoxLayout* optionsLayout = new QHBoxLayout;
    optionsLayout->setContentsMargins(8, 0, 8, 0);
    m_ClearButton = new QPushButton("Remove Completed");

    optionsLayout->addStretch(1);
    optionsLayout->addWidget(m_ClearButton);

    BaseWidget* scrollWidget = new BaseWidget;
    scrollWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    m_ScrollLayout = new QVBoxLayout(scrollWidget);
    m_ScrollLayout->setContentsMargins(4, 4, 4, 0);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollWidget);

    m_Layout->addLayout(optionsLayout);
    m_Layout->addWidget(scrollArea);
    m_Layout->setContentsMargins(1, 8, 1, 1);
}

void TaskView::Setup()
{
    m_ScrollLayout->setAlignment(Qt::AlignTop);
}

void TaskView::Connect()
{
    connect(m_ClearButton, &QPushButton::clicked, this, &TaskView::ClearCompleted);
}

VOID_NAMESPACE_CLOSE
