// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QDoubleValidator>

/* Internal */
#include "Slider.h"

VOID_NAMESPACE_OPEN

/// Frameless Slider

FramelessSlider::FramelessSlider(QWidget* parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
{
    /* Build Layout */
    Build();
    /* Connect Signals */
    connect(m_Slider, &QSlider::valueChanged, this, &FramelessSlider::valueChanged);
}

FramelessSlider::~FramelessSlider()
{
    m_Slider->deleteLater();
    m_Layout->deleteLater();
}

void FramelessSlider::Build()
{
    /* Main Layout */
    m_Layout = new QHBoxLayout(this);

    m_Slider = new QSlider(Qt::Horizontal, this);

    m_Layout->addWidget(m_Slider);
}

/// QuickDoubleSlider

QuickDoubleSlider::QuickDoubleSlider(QWidget* parent)
    : QWidget(parent)
    , m_Min(0)
    , m_Max(100)
    , m_Factor(100)
{
    Build();
    Setup();
}

QuickDoubleSlider::~QuickDoubleSlider()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void QuickDoubleSlider::SetValue(double value)
{
    m_Slider->setValue(value * m_Factor);
}

void QuickDoubleSlider::SetMinimum(double min)
{
    if (min == 0)
    {
        // m_Factor = 100;
        m_Slider->setMinimum(0);
    }
    else if (min < 0)
    {
        // m_Factor = std::abs(1 / min);
        m_Slider->setMinimum(min * m_Factor);
    }
    else
    {
        // m_Factor = 1 / min;
        m_Slider->setMinimum(1);
    }
}

void QuickDoubleSlider::SetMaximum(double max)
{
    m_Slider->setMaximum(max * m_Factor);
}

void QuickDoubleSlider::SetRange(double min, double max)
{
    SetMinimum(min);
    SetMaximum(max);
}

void QuickDoubleSlider::SetSingleStep(double step)
{
    m_Slider->setSingleStep(step * m_Factor);
}

void QuickDoubleSlider::Build()
{
    m_Layout = new QHBoxLayout(this);

    m_Editor = new QLineEdit;
    m_Slider = new QSlider(Qt::Horizontal);

    m_Layout->addWidget(m_Editor);
    m_Layout->addWidget(m_Slider);
    m_Layout->setContentsMargins(0, 0, 0, 0);
}

void QuickDoubleSlider::Setup()
{
    QDoubleValidator* validator = new QDoubleValidator;
    m_Editor->setMaximumWidth(80);
    m_Editor->setValidator(validator);

    // Default
    m_Editor->setText(QString::number(static_cast<double>(m_Slider->value()) / m_Factor));

    // Connections
    connect(m_Slider, &QSlider::valueChanged, this, [this](int value) -> void
    {
        bool blocked = m_Editor->blockSignals(true);
        m_Editor->setText(QString::number(static_cast<double>(value) / m_Factor));
        m_Editor->blockSignals(blocked);

        emit valueChanged(static_cast<double>(value) / m_Factor);
    });
}

VOID_NAMESPACE_CLOSE
