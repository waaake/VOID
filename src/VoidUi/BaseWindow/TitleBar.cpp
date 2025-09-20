// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QMouseEvent>
#include <QPainter>

/* Internal */
#include "TitleBar.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Engine/IconForge.h"

VOID_NAMESPACE_OPEN

VoidTitleBar::VoidTitleBar(QWidget* parent)
    : QWidget(parent)
    , m_Pressed(false)
{
    /* Build Menu UI */
    Build();

    /* Connect Components */
    Connect();

    /* A Default fixed height */
    setFixedHeight(40);
}

VoidTitleBar::~VoidTitleBar()
{
}

void VoidTitleBar::paintEvent(QPaintEvent* event)
{
    /* Custom Draw painter */
    QPainter painter(this);

    /* Background */
    painter.fillRect(rect(), palette().color(QPalette::Window));

    /* Standard Painting */
    QWidget::paintEvent(event);

    painter.setRenderHint(QPainter::Antialiasing);

    /* Set Pen for drawing border */
    painter.setPen(QPen(Qt::black, 2));
    /* Draw a line at the bottom of the widget */
    painter.drawLine(0, height(), width(), height());
}

void VoidTitleBar::Build()
{
    m_MenuBar = new QMenuBar;

    /* Left Corner {{{ */
    m_LeftCorner = new QWidget;
    m_LeftLayout = new QHBoxLayout(m_LeftCorner);

    m_VoidLabel = new QLabel;
    QPixmap voidPixmap(":resources/images/VOID_Logo.svg");
    m_VoidLabel->setPixmap(voidPixmap.scaled(26, 26, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_LeftLayout->addWidget(m_VoidLabel);

    /* Spacing */
    m_LeftLayout->setContentsMargins(5, 0, 0, 0);

    /* Right Side Layout */
    m_RightLayout = new QHBoxLayout;
    
    m_MinimizeButton = new QToolButton();
    m_MinimizeButton->setFixedSize(16, 16);
    m_MinimizeButton->setAutoRaise(true);
    m_MinimizeButton->setIcon(IconForge::GetIcon(IconType::icon_minimize, _DARK_COLOR(QPalette::Text, 150), 18));

    m_MaximizeButton = new QToolButton();
    m_MaximizeButton->setFixedSize(16, 16);
    m_MaximizeButton->setAutoRaise(true);
    m_MaximizeButton->setIcon(IconForge::GetIcon(IconType::icon_square, _DARK_COLOR(QPalette::Text, 150), 18));

    m_CloseButton = new CloseButton;
    m_CloseButton->setFixedSize(15, 15);

    /* Add to Layout */
    m_RightLayout->addWidget(m_MinimizeButton, 0, Qt::AlignTop);
    m_RightLayout->addWidget(m_MaximizeButton, 0, Qt::AlignTop);
    m_RightLayout->addWidget(m_CloseButton, 0, Qt::AlignTop);
    
    m_RightLayout->setContentsMargins(0, 0, 0, 0);

    /* Adjust base Layout */
    m_MenuLayout = new QHBoxLayout(this);
    m_MenuLayout->addWidget(m_LeftCorner);
    m_MenuLayout->addWidget(m_MenuBar);

    /* Stretch */
    m_MenuLayout->addStretch(1);

    /* Window Options */
    m_MenuLayout->addLayout(m_RightLayout);
}

void VoidTitleBar::Connect()
{
    connect(m_MinimizeButton, &QToolButton::clicked, this, [this]() { emit requestMinimize(); });
    connect(m_MaximizeButton, &QToolButton::clicked, this, [this]() { emit requestMaximizeRestore(); });
    connect(m_CloseButton, &CloseButton::clicked, this, [this]() { emit requestClose(); });
}

VOID_NAMESPACE_CLOSE
