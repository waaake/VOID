// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Qt */
#include <QPainter>

/* Internal */
#include "RendererStatus.h"

VOID_NAMESPACE_OPEN

/// Color Widget

ColorWidget::ColorWidget(QWidget* parent)
    : QWidget(parent)
    , m_CurrentColor(Qt::black)
{
}

void ColorWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), m_CurrentColor);
}

void ColorWidget::SetColor(const QColor& color)
{
    m_CurrentColor = color;
    update();
}

/// Renderer Diplay Label

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
    QLabel::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(Qt::black, 6));
    painter.drawRoundedRect(rect().adjusted(-3, -3, 3, 3), 10, 10);
}

void RendererDisplayLabel::Setup()
{
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::darkGray);
    palette.setColor(QPalette::WindowText, Qt::black);
    
    setAutoFillBackground(true);
    setPalette(palette);

    QFont font("Arial", 20, QFont::Light);
    setFont(font);
}

/// RendererStatusBar

RendererStatusBar::RendererStatusBar(QWidget* parent)
    : QWidget(parent)
{
    Build();
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
    m_Layout = new QHBoxLayout(this);

    m_LeftLayout = new QHBoxLayout;
    m_RightLayout = new QHBoxLayout;

    m_ResolutionLabel = new QLabel("Resolution: ");
    m_ResolutionValue = new QLabel("");

    m_XLabel = new QLabel("X=");
    m_XValue = new QLabel;
    m_YLabel = new QLabel("Y=");
    m_YValue = new QLabel;

    QPalette p = m_ResolutionLabel->palette();
    p.setColor(QPalette::WindowText, QColor(185, 185, 185));
    m_ResolutionLabel->setPalette(p);
    m_ResolutionValue->setPalette(p);
    m_XLabel->setPalette(p);
    m_XValue->setPalette(p);
    m_YLabel->setPalette(p);
    m_YValue->setPalette(p);

    m_RValue = new QLabel;
    m_GValue = new QLabel;
    m_BValue = new QLabel;
    m_AValue = new QLabel;

    p = m_RValue->palette();
    p.setColor(QPalette::WindowText, QColor(255, 0, 0));
    m_RValue->setPalette(p);

    p = m_GValue->palette();
    p.setColor(QPalette::WindowText, QColor(0, 255, 0));
    m_GValue->setPalette(p);

    p = m_BValue->palette();
    p.setColor(QPalette::WindowText, QColor(0, 0, 255));
    m_BValue->setPalette(p);

    p = m_AValue->palette();
    p.setColor(QPalette::WindowText, QColor(255, 255, 255));
    m_AValue->setPalette(p);

    m_ColorPreview = new ColorWidget;
    m_ColorPreview->setFixedSize(30, 12);

    m_LeftLayout->addWidget(m_ResolutionLabel);
    m_LeftLayout->addWidget(m_ResolutionValue);
    m_LeftLayout->addStretch(1);

    m_RightLayout->addStretch(1);
    m_RightLayout->addWidget(m_RValue);
    m_RightLayout->addWidget(m_GValue);
    m_RightLayout->addWidget(m_BValue);
    m_RightLayout->addWidget(m_AValue);
    m_RightLayout->addWidget(m_ColorPreview);

    m_Layout->addLayout(m_LeftLayout);
    m_Layout->addWidget(m_XLabel);
    m_Layout->addWidget(m_XValue);
    m_Layout->addWidget(m_YLabel);
    m_Layout->addWidget(m_YValue);
    m_Layout->addLayout(m_RightLayout);
}

void RendererStatusBar::SetRenderResolution(const int width, const int height)
{
    m_ResolutionValue->setText(QString("%1 x %2").arg(width).arg(height));
}

void RendererStatusBar::SetMouseCoordinates(const int x, const int y)
{
    m_XValue->setText(QString::number(x));
    m_YValue->setText(QString::number(y));
}

void RendererStatusBar::SetColourValues(const float r, const float g, const float b, const float a)
{
    m_RValue->setText(QString::number(r, 'f', 5));
    m_GValue->setText(QString::number(g, 'f', 5));
    m_BValue->setText(QString::number(b, 'f', 5));
    m_AValue->setText(QString::number(a, 'f', 5));

    m_ColorPreview->SetColor(QColor(r * 255, g * 255, b * 255, a * 255));
}

VOID_NAMESPACE_CLOSE
