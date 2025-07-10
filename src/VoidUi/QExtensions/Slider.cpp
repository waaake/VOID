/* Internal */
#include "Slider.h"

VOID_NAMESPACE_OPEN

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

VOID_NAMESPACE_CLOSE
