// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLabel>

/* Internal */
#include "LogWindow.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

LoggerWindow::LoggerWindow(TaskLogModel* model, QWidget* parent)
    : TranslucentDialog(parent)
{
    m_Proxy = new TaskLogProxyModel;
    m_Proxy->setSourceModel(model);

    Build();
    Connect();

    m_LogView->setModel(m_Proxy);
}

LoggerWindow::~LoggerWindow()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void LoggerWindow::Build()
{
    m_Layout = new QVBoxLayout(this);
    m_LogView = new QListView;

    QHBoxLayout* optionsLayout = new QHBoxLayout;

    m_Filter = new FilterTable({"Debug", "Info", "Warning", "Error", "Critical"}, this);

    optionsLayout->addStretch(1);
    optionsLayout->addWidget(new QLabel("Log level:"))
    optionsLayout->addWidget(m_Filter);

    m_Layout->addLayout(optionsLayout);
    m_Layout->addWidget(m_LogView);
}

void LoggerWindow::Connect()
{
    connect(m_Filter, &FilterTable::filterChanged, m_Proxy, static_cast<void (TaskLogProxyModel::*)(int)>(&TaskLogProxyModel::SetLogLevel));
}

VOID_NAMESPACE_CLOSE
