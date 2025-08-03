// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QMouseEvent>
#include <QPainter>

/* Internal */
#include "TitleBar.h"
#include "VoidCore/Logging.h"

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
    
    m_MinimizeButton = new QPushButton();
    m_MinimizeButton->setFixedSize(18, 18);
    m_MinimizeButton->setFlat(true);
    m_MinimizeButton->setIcon(QIcon(":resources/icons/icon_minimize.svg"));

    m_MaximizeButton = new QPushButton();
    m_MaximizeButton->setFixedSize(18, 18);
    m_MaximizeButton->setFlat(true);
    m_MaximizeButton->setIcon(QIcon(":resources/icons/icon_restore.svg"));

    m_CloseButton = new QPushButton();
    m_CloseButton->setFixedSize(18, 18);
    m_CloseButton->setFlat(true);
    m_CloseButton->setIcon(QIcon(":resources/icons/icon_close.svg"));

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
    connect(m_MinimizeButton, &QPushButton::clicked, this, [this]() { emit requestMinimize(); });
    connect(m_MaximizeButton, &QPushButton::clicked, this, [this]() { emit requestMaximizeRestore(); });
    connect(m_CloseButton, &QPushButton::clicked, this, [this]() { emit requestClose(); });
}

VOID_NAMESPACE_CLOSE
