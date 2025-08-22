// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cmath>

/* Qt */
#include <QMouseEvent>
#include <QPainter>

/* Internal */
#include "ControlBar.h"
#include "VoidUi/QExtensions/Frame.h"
#include "VoidUi/QExtensions/Tooltip.h"

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
    m_ExposureSpinner = new QuickSpinner;
    m_ExposureSpinner->setToolTip(
        ToolTipString(
            "Quick Exposure Adjustment Tool",
            "Adjust image brightness based on exposure values. Switches between last and the default value when the label is clicked."
        ).c_str()
    );

    m_GammaSpinner = new QuickSpinner;
    m_GammaSpinner->setToolTip(
        ToolTipString(
            "Quick Gamma Adjustment Tool",
            "Tweaks midtone luminance. Switches between last and the default value when the label is clicked."
        ).c_str()
    );

    m_GainSpinner = new QuickSpinner;
    m_GainSpinner->setToolTip(
        ToolTipString(
            "Quick Gain Adjustment Tool",
            "Amplifies image signal intensity. Switches between last and the default value when the label is clicked."
        ).c_str()
    );

    m_ChannelModeController = new ControlCombo();

    /* Annotation */
    m_AnnotationButton = new HighlightToggleButton;
    m_AnnotationButton->setIcon(QIcon(":resources/icons/icon_annotate.svg"));
    m_AnnotationButton->setFixedWidth(26);
    m_AnnotationButton->setToolTip(ToolTipString("Annotations Toolkit", "Toggles Annotation tools for the Viewer.").c_str());

    /* Viewer Display Controller */
    m_ColorDisplayController = new ColorController;

    /* Zoom Controls */
    m_Zoomer = new ControlSpinner();
    m_Zoomer->setToolTip(ToolTipString("Zoom Controller", "Adjust viewer image zoom.").c_str());

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
    m_LeftLayout->addWidget(m_ExposureSpinner);
    m_LeftLayout->addWidget(new VLine(this));
    m_LeftLayout->addWidget(m_GammaSpinner);
    m_LeftLayout->addWidget(new VLine(this));
    m_LeftLayout->addWidget(m_GainSpinner);

    /* And a spacer at the end */
    m_LeftLayout->addStretch(1);

    /* Add to the Right Layout */
    /* Spacer from the left side */
    m_RightLayout->addStretch(1);
    m_RightLayout->addWidget(m_ColorDisplayController);
    m_RightLayout->addWidget(m_Zoomer);
    m_RightLayout->addWidget(m_AnnotationButton);

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
    setFixedHeight(38);

    /**
     * Channels controller
     */
    m_ChannelModeController->addItems({"R", "G", "B", "Alpha", "RGB", "RGBA"});
    m_ChannelModeController->setCurrentIndex(5);

    /**
     * Exposure Controller
     */
    m_ExposureSpinner->SetDefault(0.f);
    m_ExposureSpinner->SetRange(-5.f, 5.f);
    m_ExposureSpinner->SetStep(0.1f);
    m_ExposureSpinner->SetLabel("E");

    /* Default */
    m_ExposureSpinner->SetValue(0.f);
    /* Resize */
    m_ExposureSpinner->setMaximumWidth(52);

    /**
     * Gamma Controller
     */
    m_GammaSpinner->SetDefault(1.0);
    m_GammaSpinner->SetRange(1.f, 3.f);
    m_GammaSpinner->SetStep(0.1f);
    m_GammaSpinner->SetLabel(QStringLiteral("γ").toStdString().c_str());

    /* Default */
    m_GammaSpinner->SetValue(1.0);
    /* Resize */
    m_GammaSpinner->setMaximumWidth(52);

    /**
     * Gain Controller
     */
    m_GainSpinner->SetDefault(1.f);
    m_GainSpinner->SetRange(0.5f, 3.5f);
    m_GainSpinner->SetStep(0.1f);
    m_GainSpinner->SetLabel("f/");

    /* Default */
    m_GainSpinner->SetValue(1.f);
    /* Resize */
    m_GainSpinner->setMaximumWidth(52);

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

    /* Viewer Controls Exposure | Gamma | Gain */
    connect(m_ExposureSpinner, &QuickSpinner::valueChanged, this, &ControlBar::exposureChanged);
    connect(m_GammaSpinner, &QuickSpinner::valueChanged, this, &ControlBar::gammaChanged);
    connect(m_GainSpinner, &QuickSpinner::valueChanged, this, &ControlBar::gainChanged);

    /* Channel Mode controller */
    connect(m_ChannelModeController, static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged), this, &ControlBar::channelModeChanged);

    /* Viewer Buffer Switch */
    connect(m_BufferSwitch, &BufferSwitch::switched, this, &ControlBar::viewerBufferSwitched);
    connect(m_BufferSwitch, &BufferSwitch::compareModeChanged, this, &ControlBar::comparisonModeChanged);
    connect(m_BufferSwitch, &BufferSwitch::blendModeChanged, this, &ControlBar::blendModeChanged);

    /* Annotations */
    connect(m_AnnotationButton, &QPushButton::toggled, this, &ControlBar::annotationsToggled);

    /* Color Display */
    connect(m_ColorDisplayController, &ColorController::colorDisplayChanged, this, &ControlBar::colorDisplayChanged);
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
