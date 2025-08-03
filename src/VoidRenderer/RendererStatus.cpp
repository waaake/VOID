// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Qt */
#include <QPainter>

/* Internal */
#include "RendererStatus.h"

VOID_NAMESPACE_OPEN

/* Color Widget {{{ */
ColorWidget::ColorWidget(QWidget* parent)
    : QWidget(parent)
    , m_CurrentColor(Qt::black)
{
}

void ColorWidget::paintEvent(QPaintEvent* event)
{
    /* Construct a painter to color the widget */
    QPainter painter(this);

    /* Fill the widget with the current color */
    painter.fillRect(rect(), m_CurrentColor);
}

void ColorWidget::SetColor(const QColor& color)
{
    m_CurrentColor = color;
    /* Repaint */
    update();
}
/* }}} */

/* Renderer Diplay Label {{{ */
RendererDisplayLabel::RendererDisplayLabel(QWidget* parent)
    : QLabel(parent)
{
    Setup();
}

RendererDisplayLabel::RendererDisplayLabel(const std::string& text, QWidget* parent)
    : QLabel(text.c_str(), parent)
{
    Setup();
}

void RendererDisplayLabel::paintEvent(QPaintEvent* event)
{
    /* Default Draw */
    QLabel::paintEvent(event);

    /* Painter to draw */
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    /* Add Rounded Border */
    painter.setPen(QPen(Qt::black, 6));
    painter.drawRoundedRect(rect().adjusted(-3, -3, 3, 3), 10, 10);
}

void RendererDisplayLabel::Setup()
{
    /* Setup how the Label and text appears */
    QPalette palette;
    /* Setup background */
    palette.setColor(QPalette::Window, Qt::darkGray);
    /* Text */
    palette.setColor(QPalette::WindowText, Qt::black);
    
    setAutoFillBackground(true);
    setPalette(palette);

    /* Font Face */
    QFont font("Arial", 20, QFont::Light);
    setFont(font);
}
/* }}} */

/* RendererStatusBar {{{ */
RendererStatusBar::RendererStatusBar(QWidget* parent)
    : QWidget(parent)
{
    /* Build the base layout */
    Build();

    /* Set a Fixed height for the Status Bar*/
    setFixedHeight(30);
}

RendererStatusBar::~RendererStatusBar()
{
    m_LeftLayout->deleteLater();
    delete m_LeftLayout;
    m_LeftLayout = nullptr;

    m_RightLayout->deleteLater();
    delete m_RightLayout;
    m_RightLayout = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void RendererStatusBar::Build()
{
    /* Base Layout */
    m_Layout = new QHBoxLayout(this);

    m_LeftLayout = new QHBoxLayout;
    m_RightLayout = new QHBoxLayout;

    m_ResolutionLabel = new QLabel("Resolution: ");
    m_ResolutionValue = new QLabel("");

    m_XLabel = new QLabel("X=");
    m_XValue = new QLabel;
    m_YLabel = new QLabel("Y=");
    m_YValue = new QLabel;

    m_RValue = new QLabel;
    m_GValue = new QLabel;
    m_BValue = new QLabel;
    m_AValue = new QLabel;

    /* Red Color */
    QPalette p = m_RValue->palette();
    p.setColor(QPalette::WindowText, QColor(255, 0, 0));
    m_RValue->setPalette(p);

    /* Green Color */
    p = m_GValue->palette();
    p.setColor(QPalette::WindowText, QColor(0, 255, 0));
    m_GValue->setPalette(p);

    /* Blue Color */
    p = m_BValue->palette();
    p.setColor(QPalette::WindowText, QColor(0, 0, 255));
    m_BValue->setPalette(p);

    /* Alpha Color */
    p = m_AValue->palette();
    p.setColor(QPalette::WindowText, QColor(255, 255, 255));
    m_AValue->setPalette(p);

    m_ColorPreview = new ColorWidget;
    /* Fixed Size */
    m_ColorPreview->setFixedSize(30, 12);

    /* Left Layout */
    m_LeftLayout->addWidget(m_ResolutionLabel);
    m_LeftLayout->addWidget(m_ResolutionValue);
    m_LeftLayout->addStretch(1);

    /* Right Layout */
    m_RightLayout->addStretch(1);
    m_RightLayout->addWidget(m_RValue);
    m_RightLayout->addWidget(m_GValue);
    m_RightLayout->addWidget(m_BValue);
    m_RightLayout->addWidget(m_AValue);
    m_RightLayout->addWidget(m_ColorPreview);

    /* Add to the main layout */
    /* Left */
    m_Layout->addLayout(m_LeftLayout);

    /* Centered */
    m_Layout->addWidget(m_XLabel);
    m_Layout->addWidget(m_XValue);
    m_Layout->addWidget(m_YLabel);
    m_Layout->addWidget(m_YValue);

    /* Right */
    m_Layout->addLayout(m_RightLayout);
}

void RendererStatusBar::SetRenderResolution(const int width, const int height)
{
    std::stringstream ss; 
    ss << std::to_string(width) << " x " << std::to_string(height);

    /* Update the Resolution display */
    m_ResolutionValue->setText(ss.str().c_str());
}

void RendererStatusBar::SetMouseCoordinates(const int x, const int y)
{
    m_XValue->setText(std::to_string(x).c_str());
    m_YValue->setText(std::to_string(y).c_str());
}

void RendererStatusBar::SetColourValues(const float r, const float g, const float b, const float a)
{
    /* Update color labels */
    m_RValue->setText(std::to_string(r).c_str());
    m_GValue->setText(std::to_string(r).c_str());
    m_BValue->setText(std::to_string(r).c_str());
    m_AValue->setText(std::to_string(a).c_str());

    /* Update color preview */
    m_ColorPreview->SetColor(QColor(r * 255, g * 255, b * 255, a * 255));
}
/* }}} */

VOID_NAMESPACE_CLOSE
