/* STD */
#include <cmath>

/* Qt */
#include <QMouseEvent>
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

ControlBar::ControlBar(ViewerBuffer* A, ViewerBuffer* B, QWidget* parent)
    : QWidget(parent)
    , m_ViewerBufferA(A)
    , m_ViewerBufferB(B)
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

void ControlBar::paintEvent(QPaintEvent* event)
{
    /* Default painting */
    QWidget::paintEvent(event);

    /* Create a Painter to draw the border */
    QPainter painter(this);

    /* Set Draw requisites */
    painter.setPen(QPen(Qt::black, 2));

    /* Draw the border */
    painter.drawLine(0, 0, width(), 0);
}

QFrame* ControlBar::Separator()
{
    QFrame* separator = new QFrame;
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setLineWidth(0);
    separator->setMidLineWidth(2);

    return separator;
}

void ControlBar::Build()
{
    /* Main Layout */
    m_Layout = new QHBoxLayout(this);

    /* Viewer Buffer Controls */
    m_BufferSwitch = new BufferSwitch(m_ViewerBufferA, m_ViewerBufferB);

    /* Viewer Controls */
    m_ExposureLabel = new QLabel("E");
    m_ExposureController = new ControlDoubleSpinner();

    m_GammaLabel = new QLabel(QStringLiteral("γ"));
    m_GammaController = new ControlDoubleSpinner();

    m_GainLabel = new QLabel("f/");
    m_GainController = new ControlDoubleSpinner();

    m_ChannelModeController = new ControlCombo();

    /* Zoom Controls */
    m_Zoomer = new ControlSpinner();

    /* The Left Side Widget */
    m_LeftControls = new QWidget();
    m_LeftLayout = new QHBoxLayout(m_LeftControls);
    m_LeftLayout->setContentsMargins(0, 0, 0, 0);

    /* Right Side Widget */
    m_RightControls = new QWidget();
    m_RightLayout = new QHBoxLayout(m_RightControls);
    m_RightLayout->setContentsMargins(0, 0, 0, 0);

    /* Add to Left Controls */
    m_LeftLayout->addWidget(m_ChannelModeController);
    m_LeftLayout->addWidget(m_ExposureLabel);
    m_LeftLayout->addWidget(m_ExposureController);
    m_LeftLayout->addWidget(m_GammaLabel);
    m_LeftLayout->addWidget(m_GammaController);
    m_LeftLayout->addWidget(m_GainLabel);
    m_LeftLayout->addWidget(m_GainController);

    /* And a spacer at the end */
    m_LeftLayout->addStretch(1);

    /* Add to the Right Layout */
    /* Spacer from the left side */
    m_RightLayout->addStretch(1);
    m_RightLayout->addWidget(m_Zoomer);

    /* Add the left side controls */
    m_Layout->addWidget(m_LeftControls);
    /* The Buffer switch in the middle */
    m_Layout->addWidget(m_BufferSwitch);
    /* And the Right side controls */
    m_Layout->addWidget(m_RightControls);
}

void ControlBar::Setup()
{
    /* Widget has a Fixed height */
    setFixedHeight(40);

    /**
     * Channels controller
     */
    m_ChannelModeController->addItems({"R", "G", "B", "Alpha", "RGB", "RGBA"});
    m_ChannelModeController->setCurrentIndex(5);

    /**
     * Exposure Controller
     */
    m_ExposureController->setMinimum(-5.f);
    m_ExposureController->setMaximum(5.f);
    m_ExposureController->setSingleStep(0.1f);
    m_ExposureController->setDecimals(1);

    /* Default */
    m_ExposureController->setValue(0.f);

    /**
     * Gamma Controller
     */
    m_GammaController->setMinimum(1.f);
    m_GammaController->setMaximum(3.f);
    m_GammaController->setSingleStep(0.1f);
    m_GammaController->setDecimals(1);

    /* Default */
    m_GammaController->setValue(1.f);

    /**
     * Gain Controller
     */
    m_GainController->setMinimum(0.5f);
    m_GainController->setMaximum(3.f);
    m_GainController->setSingleStep(0.1f);
    m_GainController->setDecimals(1);

    /* Default */
    m_GainController->setValue(1.f);

    /**
     * Zoom Slider
     * QSlider operates only on a Linear Scale
     * But since the zoom requires values ranging from 0.1f to 12.8f
     * We would want to map the value from slider onto the zoom range
     * And also from zoom range to slider value
     */
    m_Zoomer->setMinimum(1);
    m_Zoomer->setMaximum(100);

    /* 50 acts as the mid way for the slider which gets mapped to a zoom of 1.f */
    m_Zoomer->setValue(50);
}

void ControlBar::Connect()
{
    /* Zoom */
    connect(m_Zoomer, static_cast<void (QSpinBox::* )(int)>(&QSpinBox::valueChanged), this, &ControlBar::UpdateZoom);

    /* Viewer Controls Exposure | Gamma */
    connect(m_ExposureController, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ControlBar::exposureChanged);
    connect(m_GammaController, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ControlBar::gammaChanged);
    connect(m_GainController, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ControlBar::gainChanged);

    /* Channel Mode controller */
    connect(m_ChannelModeController, static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged), this, &ControlBar::channelModeChanged);

    /* Viewer Buffer Switch */
    connect(m_BufferSwitch, &BufferSwitch::switched, this, &ControlBar::viewerBufferSwitched);
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
    bool blocked = m_Zoomer->blockSignals(true);

    /* Cast the zoom to the slider space */
    m_Zoomer->setValue(MapFromZoom(zoom));

    /* Unblock signals */
    m_Zoomer->blockSignals(blocked);
}

VOID_NAMESPACE_CLOSE
