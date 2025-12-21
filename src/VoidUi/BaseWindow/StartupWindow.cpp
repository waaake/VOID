// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Qt */
#include <QPainter>

/* Internal */
#include "StartupWindow.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Preferences/Preferences.h"
#include "VoidUi/QExtensions/Delegates.h"
#include "VoidUi/Project/ProjectBridge.h"
#include "VoidUi/Project/Delegates/ListDelegate.h"

VOID_NAMESPACE_OPEN

LogoWidget::LogoWidget(QWidget* parent)
{
}

void LogoWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
}

StartupWindow::StartupWindow(QWidget* parent)
    : QDialog(parent)
{
    Build();
    Setup();
    Connect();
    Populate();
}

StartupWindow::~StartupWindow()
{
    m_SideButtonsLayout->deleteLater();
    delete m_SideButtonsLayout;
    m_SideButtonsLayout = nullptr;

    m_BottomButtonsLayout->deleteLater();
    delete m_BottomButtonsLayout;
    m_BottomButtonsLayout = nullptr;

    m_ProjectsLayout->deleteLater();
    delete m_ProjectsLayout;
    m_ProjectsLayout = nullptr;

    m_InternalSplitLayout->deleteLater();
    delete m_InternalSplitLayout;
    m_InternalSplitLayout = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;

    m_Projects->deleteLater();
    delete m_Projects;
    m_Projects = nullptr;
}

void StartupWindow::Build()
{
    m_Layout = new QVBoxLayout(this);

    m_InternalSplitLayout = new QHBoxLayout;
    m_ProjectsLayout = new QVBoxLayout;
    m_SideButtonsLayout = new QVBoxLayout;
    m_BottomButtonsLayout = new QHBoxLayout;

    LogoWidget* labelWidget = new LogoWidget(this);
    QHBoxLayout* labelLayout = new QHBoxLayout(labelWidget);

    QLabel* voidLabel = new QLabel;
    QImage logo(":resources/images/VOID_Logo_900x200.png");
    voidLabel->setPixmap(QPixmap::fromImage(logo.scaledToWidth(150, Qt::SmoothTransformation)));

    labelLayout->addWidget(voidLabel);
    labelWidget->setContentsMargins(0, 0, 0, 0);

    m_NewProjectBtn = new QPushButton("Start a New Project");
    m_OpenProjectBtn = new QPushButton("Open a Project...");

    m_SideButtonsLayout->addWidget(m_NewProjectBtn);
    m_SideButtonsLayout->addWidget(m_OpenProjectBtn);
    m_SideButtonsLayout->addStretch(1);

    QLabel* recentProjectsLabel = new QLabel("Recent Projects");
    m_ProjectsLister = new QListView;

    m_DontShowCheck = new QCheckBox("Don't Show this dialog on startup");
    m_LoadBtn = new QPushButton("Load Selected");
    m_CloseBtn = new QPushButton("Close");

    m_BottomButtonsLayout->addWidget(m_DontShowCheck);
    m_BottomButtonsLayout->addStretch(1);
    m_BottomButtonsLayout->addWidget(m_LoadBtn);
    m_BottomButtonsLayout->addWidget(m_CloseBtn);

    m_ProjectsLayout->addWidget(recentProjectsLabel);
    m_ProjectsLayout->addWidget(m_ProjectsLister);

    m_InternalSplitLayout->addLayout(m_SideButtonsLayout);
    m_InternalSplitLayout->addLayout(m_ProjectsLayout);

    m_Layout->addWidget(labelWidget);
    m_Layout->addLayout(m_InternalSplitLayout);
    m_Layout->addLayout(m_BottomButtonsLayout);
}

void StartupWindow::Connect()
{
    connect(m_CloseBtn, &QPushButton::clicked, this, &StartupWindow::close);
    connect(m_LoadBtn, &QPushButton::clicked, this, &StartupWindow::LoadSelected);
    connect(m_OpenProjectBtn, &QPushButton::clicked, this, [this]()
    {
        _ProjectBridge.Open();
        Close();
    });
    connect(m_NewProjectBtn, &QPushButton::clicked, this, [this]()
    {
        _ProjectBridge.New();
        Close();
    });

    connect(m_ProjectsLister, &QListView::doubleClicked, this, &StartupWindow::LoadSelected);
}

void StartupWindow::Setup()
{
    m_Projects = new RecentProjectsModel(this);
    m_ProjectsLister->setModel(m_Projects);
    m_ProjectsLister->setItemDelegate(new RecentProjectItemDelegate(this));
    m_ProjectsLister->setSpacing(1);
}

void StartupWindow::Populate()
{
    m_Projects->Add(VoidPreferences::Instance().RecentProjects());
}

void StartupWindow::LoadSelected()
{

    const std::filesystem::path& project = m_Projects->Project(m_ProjectsLister->selectionModel()->currentIndex());
    if (project.empty())
    {
        VOID_LOG_INFO("No Project selected to Load");
        return;
    }

    VOID_LOG_INFO("Loading Recent Project: {0}", project.string());

    _ProjectBridge.Open(project.string());
    Close();
}

void StartupWindow::Exec(QWidget* parent)
{
    /* Check if the preferences allow to show the startup*/
    if (VoidPreferences::Instance().ShowStartup())
        StartupWindow(parent).exec();
}

void StartupWindow::Close()
{
    /* Ensure that the state of whether the dialog needs to be shown again or not is saved in the prefs */
    if (m_DontShowCheck->isChecked())
        VoidPreferences::Instance().Set(Settings::DontShowStartup, true);

    close();
}

VOID_NAMESPACE_CLOSE
