// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ProgressTask.h"

VOID_NAMESPACE_OPEN

ProgressTask::ProgressTask(QWidget* parent)
    : QDialog(parent)
{
    Build();
    Setup();
    Connect();
}

ProgressTask::~ProgressTask()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void ProgressTask::Build()
{
    m_Layout = new QVBoxLayout(this);

    m_ButtonLayout = new QHBoxLayout;
    m_TaskLayout = new QHBoxLayout;

    m_TaskTypeLabel = new QLabel;
    m_CurrentTaskLabel = new QLabel;

    m_TaskLayout->addWidget(m_TaskTypeLabel);
    m_TaskLayout->addStretch(1);
    m_TaskLayout->addWidget(m_CurrentTaskLabel);

    m_ProgressBar = new QProgressBar;
    m_CancelButton = new QPushButton("Cancel");

    m_ButtonLayout->addStretch(1);
    m_ButtonLayout->addWidget(m_CancelButton);

    m_Layout->addLayout(m_TaskLayout);
    m_Layout->addWidget(m_ProgressBar);
    m_Layout->addLayout(m_ButtonLayout);
}

void ProgressTask::Setup()
{
    m_ProgressBar->setAlignment(Qt::AlignCenter);
    m_CurrentTaskLabel->setFixedWidth(180);
}

void ProgressTask::Cancel()
{
    m_Cancelled = true;
    
    m_TaskTypeLabel->setText("Cancelling...");
    m_CancelButton->setEnabled(false);

    emit cancelled();
}

void ProgressTask::Connect()
{
    connect(m_CancelButton, &QPushButton::clicked, this, &ProgressTask::Cancel);
}

void ProgressTask::SetLabelTask(const char* text)
{
    QFontMetrics fm = m_CurrentTaskLabel->fontMetrics();
    m_CurrentTaskLabel->setText(fm.elidedText(text, Qt::ElideLeft, m_CurrentTaskLabel->width()));
}

VOID_NAMESPACE_CLOSE
