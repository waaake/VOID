// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
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

/// Simple Slider

SimpleSlider::SimpleSlider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent)
{
}

void SimpleSlider::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // Groove
    painter.setPen(Qt::black);
    painter.setBrush(palette().color(QPalette::Base));
    painter.drawRect(0, height() * 0.25, width(), 4);

    if (tickInterval())
    {
        for (int i = minimum(); i <= maximum(); i+= tickInterval())
        {
            painter.setPen(QPen(Qt::black, 1));
            int pos = width() * (i - minimum()) / (maximum() - minimum());
    
            painter.drawLine(pos, height() * 0.5 + 4, pos, height());
        }
    }

    // Handle
    int hpos = width() * (value() - minimum()) / std::max((maximum() - minimum()), 1);
    painter.setPen(Qt::black);

    painter.setBrush(isSliderDown() ? palette().color(QPalette::Highlight) : QColor(210, 210, 210));
    painter.drawRect(hpos - 2, height() * 0.15 , 4, height() - (height() * 0.5));
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
    // Ensure that we are working within the integer bounds i.e. not in decimal
    CalculateFactor(min);

    if (min == 0)
        m_Slider->setMinimum(0);
    else if (min < 0)
        m_Slider->setMinimum(min * m_Factor);
    else
        m_Slider->setMinimum(1);
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
    m_Slider = new SimpleSlider(Qt::Horizontal);

    m_Layout->addWidget(m_Editor);
    m_Layout->addWidget(m_Slider);
    m_Layout->setContentsMargins(0, 0, 0, 0);
}

void QuickDoubleSlider::Setup()
{
    QDoubleValidator* validator = new QDoubleValidator;
    m_Editor->setMaximumWidth(80);
    m_Editor->setValidator(validator);

    m_Slider->setTickPosition(QSlider::TicksBelow);
    m_Slider->setTickInterval(m_Slider->singleStep() * m_Factor);

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
    connect(m_Editor, &QLineEdit::returnPressed, this, &QuickDoubleSlider::EditorUpdated);
    connect(m_Editor, &QLineEdit::editingFinished, this, &QuickDoubleSlider::EditorUpdated);
}

void QuickDoubleSlider::EditorUpdated()
{
    QString text = m_Editor->text();
    // User decided to leave this empty, in that case we want to restore the last value from the slider
    if (text.isEmpty())
    {
        m_Editor->setText(QString::number(static_cast<double>(m_Slider->value()) / m_Factor));
        return;
    }

    m_Slider->setValue(text.toDouble() * m_Factor);
}

void QuickDoubleSlider::CalculateFactor(double minimum)
{
    if (minimum != 0 && std::abs(minimum * m_Factor) < 1)
        m_Factor = 1 / minimum;
}

VOID_NAMESPACE_CLOSE
