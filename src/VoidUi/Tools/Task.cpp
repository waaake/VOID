// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "Task.h"
#include "LogWindow.h"
#include "VoidUi/Engine/IconForge.h"

VOID_NAMESPACE_OPEN

TaskWidget::TaskWidget(Task* task, QWidget* parent)
    : QWidget(parent)
    , m_Task(task)
    , m_ActionState(ActionState::Cancel)
{
    Build();
    Connect();
    Setup();
}

TaskWidget::~TaskWidget()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;

    if (m_Task)
    {
        m_Task->deleteLater();
        delete m_Task;
        m_Task = nullptr;
    }
}

void TaskWidget::Close()
{
    hide();
    setParent(nullptr);
    deleteLater();
}

void TaskWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().color(QPalette::AlternateBase));
}

void TaskWidget::Build()
{
    m_Layout = new QGridLayout(this);

    m_TaskLabel = new QLabel;
    m_ActionButton = new QToolButton;

    m_StateLabel = new QLabel;
    m_ProgressBar = new QProgressBar;
    m_LogButton = new QToolButton;

    m_Layout->addWidget(m_TaskLabel, 0, 0, 1, 10, Qt::AlignLeft);
    m_Layout->addWidget(m_ActionButton, 0, 10, 1, 1, Qt::AlignRight);
    m_Layout->addWidget(m_StateLabel, 1, 0, 1, 2, Qt::AlignLeft);
    m_Layout->addWidget(m_ProgressBar, 1, 7, 1, 3, Qt::AlignRight);
    m_Layout->addWidget(m_LogButton, 1, 10, 1, 1, Qt::AlignRight);
}

void TaskWidget::Connect()
{
    connect(m_Task, &Task::stateChanged, this, &TaskWidget::UpdateState);
    connect(m_Task, &Task::finished, this, &TaskWidget::Finished);
    connect(m_Task, &Task::maxUpdated, this, [this](int max) -> void
    {
        m_ProgressBar->setMinimum(0);
        m_ProgressBar->setMaximum(max);

        m_ProgressBar->setValue(0);
        m_ProgressBar->setVisible(true);
    });
    connect(m_Task, &Task::progressed, this, [this](int steps) -> void
    {
        m_ProgressBar->setValue(steps);
    });

    connect(m_ActionButton, &QToolButton::clicked, this, [this]() -> void
    {
        m_ActionState == ActionState::Cancel ? Cancel() : Restart();
    });
    connect(m_LogButton, &QToolButton::clicked, this, &TaskWidget::ShowLog);
}

void TaskWidget::Setup()
{
    m_TaskLabel->setText(Label());
    m_LogButton->setIcon(IconForge::GetIcon(IconType::icon_assignment, _DARK_COLOR(QPalette::Text, 100)));
    m_ProgressBar->setVisible(false);

    m_ActionButton->setAutoRaise(true);
    m_LogButton->setAutoRaise(true);

    UpdateActionState(ActionState::Cancel);
    UpdateState(m_Task->State());
    setFixedHeight(70);
}

void TaskWidget::UpdateState(const TaskState& state)
{
    QFont f(m_StateLabel->font());
    f.setBold(true);

    QPalette p(m_StateLabel->palette());

    switch (state)
    {
        case TaskState::Running:
            p.setColor(QPalette::WindowText, QColor(50, 210, 230));
            m_StateLabel->setText("In progress");
            break;
        case TaskState::Failed:
            p.setColor(QPalette::WindowText, QColor(230, 50, 50));
            m_StateLabel->setText("Failed");
            break;
        case TaskState::Success:
            p.setColor(QPalette::WindowText, QColor(120, 230, 50));
            m_StateLabel->setText("Success");
            break;
        case TaskState::Cancelling:
            p.setColor(QPalette::WindowText, QColor(200, 200, 200));
            m_StateLabel->setText("Cancelling");
            break;
        case TaskState::Cancelled:
            p.setColor(QPalette::WindowText, QColor(200, 200, 200));
            m_StateLabel->setText("Cancelled");
            break;
        case TaskState::Queued:
        default:
            p.setColor(QPalette::WindowText, QColor(200, 200, 200));
            m_StateLabel->setText("Queued");
    }

    m_StateLabel->setPalette(p);
    m_StateLabel->setFont(f);
}

QString TaskWidget::Label()
{
    QString name = m_Task->Name().c_str();
    QString label = m_Task->Label().c_str();

    if (label.isEmpty())
        return name;

    return QString("%1 - %2").arg(name).arg(label);
}

void TaskWidget::Cancel()
{
    if (!m_Task || m_Task->Completed())
        return;

    UpdateState(m_Task->Started() ? TaskState::Cancelling : TaskState::Cancelled);
    m_Task->Cancel();
    UpdateActionState(ActionState::Retry);
}

void TaskWidget::Restart()
{
    if (m_Task)
    {
        emit restartRequested(m_Task);
        UpdateState(TaskState::Queued);
        UpdateActionState(ActionState::Cancel);
    }
}

void TaskWidget::Finished(const TaskState& state)
{
    UpdateState(state);

    if (state == TaskState::Success)
    {
        m_ProgressBar->setValue(m_ProgressBar->maximum());

        /// Check if we can do something about this, and show logger too
        // m_Task->deleteLater();
        // delete m_Task;
        // m_Task = nullptr;
    }
    else if (state == TaskState::Failed || state == TaskState::Cancelled)
    {
        UpdateActionState(ActionState::Retry);
    }

    m_ProgressBar->setVisible(false);
}

void TaskWidget::UpdateActionState(const ActionState& state)
{
    m_ActionState = state;
    m_ActionButton->setIcon(IconForge::GetIcon(
        state == ActionState::Cancel ? IconType::icon_close : IconType::icon_restart_alt,
        _DARK_COLOR(QPalette::Text, 100)));
}

void TaskWidget::ShowLog()
{
    LoggerWindow(m_Task->Logs(), this).exec();
} 

VOID_NAMESPACE_CLOSE
