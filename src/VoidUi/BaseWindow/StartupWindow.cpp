// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "StartupWindow.h"

VOID_NAMESPACE_OPEN

StartupWindow::StartupWindow(QWidget* parent)
    : QDialog(parent)
{
    Build();
    Connect();
}

StartupWindow::~StartupWindow()
{
    m_SideButtonsLayout->deleteLater();
    delete m_SideButtonsLayout;
    m_SideButtonsLayout = nullptr;

    m_BottomButtonsLayout->deleteLater();
    delete m_BottomButtonsLayout;
    m_BottomButtonsLayout = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void StartupWindow::Build()
{
    m_Layout = new QVBoxLayout(this);

    m_InternalSplitLayout = new QHBoxLayout;
    m_SideButtonsLayout = new QVBoxLayout;
    m_BottomButtonsLayout = new QHBoxLayout;

    QLabel* logo = new QLabel("VOID");
    QFont f = logo->font();
    f.setPixelSize(36);
    logo->setFont(f);
    
    m_NewProjectBtn = new QPushButton("Start a New Project");
    m_OpenProjectBtn = new QPushButton("Open a Project...");
    
    m_SideButtonsLayout->addWidget(m_NewProjectBtn);
    m_SideButtonsLayout->addWidget(m_OpenProjectBtn);
    m_SideButtonsLayout->addStretch(1);

    m_ProjectsLister = new QListWidget;

    m_LoadBtn = new QPushButton("Load Selected");
    m_CloseBtn = new QPushButton("Close");

    m_BottomButtonsLayout->addStretch(1);
    m_BottomButtonsLayout->addWidget(m_LoadBtn);
    m_BottomButtonsLayout->addWidget(m_CloseBtn);

    m_InternalSplitLayout->addLayout(m_SideButtonsLayout);
    m_InternalSplitLayout->addWidget(m_ProjectsLister);

    m_Layout->addWidget(logo);
    m_Layout->addLayout(m_InternalSplitLayout);
    m_Layout->addLayout(m_BottomButtonsLayout);
}

void StartupWindow::Connect()
{

}

VOID_NAMESPACE_CLOSE
