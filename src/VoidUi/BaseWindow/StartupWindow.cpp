// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "StartupWindow.h"
#include "VoidUi/Preferences/Preferences.h"
#include "VoidUi/QExtensions/Delegates.h"

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

    m_ProjectsLister = new QListWidget;

    m_DontShowCheck = new QCheckBox("Don't Show this Dialog on Startup");
    m_LoadBtn = new QPushButton("Load Selected");
    m_CloseBtn = new QPushButton("Close");

    m_BottomButtonsLayout->addWidget(m_DontShowCheck);
    m_BottomButtonsLayout->addStretch(1);
    m_BottomButtonsLayout->addWidget(m_LoadBtn);
    m_BottomButtonsLayout->addWidget(m_CloseBtn);

    m_InternalSplitLayout->addLayout(m_SideButtonsLayout);
    m_InternalSplitLayout->addWidget(m_ProjectsLister);

    m_Layout->addWidget(labelWidget);
    m_Layout->addLayout(m_InternalSplitLayout);
    m_Layout->addLayout(m_BottomButtonsLayout);
}

void StartupWindow::Connect()
{
    connect(m_CloseBtn, &QPushButton::clicked, this, &StartupWindow::close);
}

void StartupWindow::Populate()
{
    m_ProjectsLister->setItemDelegate(new HCustomItemDelegate(40));
    for (const std::string& project : VoidPreferences::Instance().RecentProjects())
    {
        QListWidgetItem* item = new QListWidgetItem(m_ProjectsLister);
        item->setText(project.c_str());
        // item->setSizeHint(QSize(item->sizeHint().width(), 80));

        m_ProjectsLister->addItem(item);
    }
}

VOID_NAMESPACE_CLOSE
