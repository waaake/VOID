/* STD */
#include <cmath>

/* Qt */
#include <QPainter>

/* Internal */
#include "ControlBar.h"

VOID_NAMESPACE_OPEN

ControlSlider::ControlSlider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent)
{
}

ControlSlider::~ControlSlider()
{
}

void ControlSlider::paintEvent(QPaintEvent* event)
{
    /* Painter to draw the slider */
    QPainter painter(this);

    /* Width of each unit value represented in the slider */
    double uwidth = double(width()) / (maximum() - minimum());

    /* Groove {{{ */
    painter.setPen(QColor(20, 20, 20));
    painter.setBrush(QColor(50, 50, 50));

    painter.drawRect(0, height() / 2, width(), 4);
    /* }}} */

    /* Markers {{{ */
    int step = 10;

    for (int i = minimum(); i <= maximum(); i+= step)
    {
        painter.setPen(QPen(QColor(20, 20, 20), 2));
        int pos = width() * (i - minimum()) / (maximum() - minimum());
        /* Draw Line representing Markers */
        painter.drawLine(pos, height() / 2, pos, 0);
    }

    /* Position Handle */
    int hpos = width() * (value() - minimum()) / std::max((maximum() - minimum()), 1);
    painter.setPen(QColor(20, 20, 20));
    painter.setBrush(QColor(210, 210, 210));
    painter.drawRect(hpos - 2, 0, 4, height());
    /* }}} */
}

ControlBar::ControlBar(QWidget* parent)
    : QWidget(parent)
{
    /* Build UI */
    Build();

    /* Setup how the UI appears */
    Setup();

    /* Connect Signals from Components */
    Connect();
}

ControlBar::~ControlBar()
{
}

void ControlBar::Build()
{
    /* Main Layout */
    m_Layout = new QHBoxLayout(this);

    /* Zoom Controls */
    m_ZoomInButton = new QPushButton();
    m_ZoomInButton->setIcon(QIcon("resources/icons/icon_zoom_in.svg"));
    m_ZoomInButton->setFixedWidth(26);

    m_ZoomOutButton = new QPushButton();
    m_ZoomOutButton->setFixedWidth(26);
    m_ZoomOutButton->setIcon(QIcon("resources/icons/icon_zoom_out.svg"));

    m_ZoomSlider = new ControlSlider(Qt::Horizontal);

    /* Add to the main layout */
    m_Layout->addStretch(1);

    m_Layout->addWidget(m_ZoomOutButton);
    m_Layout->addWidget(m_ZoomSlider);
    m_Layout->addWidget(m_ZoomInButton);
}

void ControlBar::Setup()
{
    /* Widget has a Fixed height */
    setFixedHeight(40);

    /*
     * Zoom Slider
     * QSlider operates only on a Linear Scale
     * But since the zoom requires values ranging from 0.1f to 12.8f
     * We would want to map the value from slider onto the zoom range
     * And also from zoom range to slider value
     */
    m_ZoomSlider->setMinimum(1);
    m_ZoomSlider->setMaximum(100);
    m_ZoomSlider->setPageStep(1);

    /* 50 acts as the mid way for the slider which gets mapped to a zoom of 1.f */
    m_ZoomSlider->setValue(50);

    m_ZoomSlider->setMinimumWidth(150);
}

void ControlBar::Connect()
{
    connect(m_ZoomSlider, &QSlider::valueChanged, this, &ControlBar::UpdateZoom);
    connect(m_ZoomInButton, &QPushButton::clicked, this, &ControlBar::ZoomIn);
    connect(m_ZoomOutButton, &QPushButton::clicked, this, &ControlBar::ZoomOut);
}

float ControlBar::MapToZoom(int value)
{
    /*
     * QSlider operates on a Linear Scale and so is the ZoomSlider
     * The range of the ZoomSlider is from 1 -> 100
     * This method maps the values from that range into 0.1f - 1.f - 12.8f
     * Where 1 from the Slider is mapped as 0.1f and 50 as 1.f, 100 as 12.8f
     * 
     * To ensure the second half of the slider also has similar precision as the first half
     * We use logarithmic scaling
     */

    if (value <= 50)
        return 0.1f + (value / 50.f) * (1.f - 0.1f);
    
    /* Second half of the slider */
    float normalized = (value - 50) / 50.f;
    /* Base 10 */
    return std::pow(10.f, normalized * (std::log10(12.8) - std::log10(1.f))) * 1.f;
}

int ControlBar::MapFromZoom(float zoom)
{
    /* First Half */
    /* This casts the value into slider range of 1 - 50 */
    if (zoom <= 1.f)
        return static_cast<int>((zoom - 0.1f) / (1.f - 0.1f) * 50);
    
    /* Casts the value into slider range of 51 - 100 */
    return 50 + static_cast<int>(50 * (std::log10(zoom)- std::log10(1.f)) / std::log10(12.8) - std::log10(1.f));
}

void ControlBar::SetFromZoom(float zoom)
{
    /* Block Signals */
    bool blocked = m_ZoomSlider->blockSignals(true);

    /* Cast the zoom to the slider space */
    m_ZoomSlider->setValue(MapFromZoom(zoom));

    /* Unblock signals */
    m_ZoomSlider->blockSignals(blocked);
}

VOID_NAMESPACE_CLOSE
