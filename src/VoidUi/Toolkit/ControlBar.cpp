// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cmath>

/* Qt */
#include <QMouseEvent>
#include <QPainter>

/* Internal */
#include "ControlBar.h"
#include "VoidUi/Engine/IconForge.h"
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
    // double uwidth = double(width()) / (maximum() - minimum());

    /* Groove {{{ */
    painter.setPen(QColor(20, 20, 20));
    painter.setBrush(QColor(50, 50, 50));

    painter.drawRect(0, height() * 0.5, width(), 4);
    /* }}} */

    /* Markers {{{ */
    // int step = 10;

    for (int i = minimum(); i <= maximum(); i+= 10)
    {
        painter.setPen(QPen(QColor(20, 20, 20), 2));
        int pos = width() * (i - minimum()) / (maximum() - minimum());
        /* Draw Line representing Markers */
        painter.drawLine(pos, height() * 0.5, pos, 0);
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
    m_AnnotationButton->setIcon(IconForge::GetIcon(IconType::icon_draw, _DARK_COLOR(QPalette::Text, 100)));
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

    m_Zoomer->setMinimum(1);
    m_Zoomer->setMaximum(100);
    m_Zoomer->setValue(50);
}

void ControlBar::Connect()
{
    /* Zoom */
    connect(m_Zoomer, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ControlBar::zoomChanged);

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

void ControlBar::SetZoom(float zoom)
{ 
    bool blocked = m_Zoomer->blockSignals(true);
    m_Zoomer->setValue(zoom);
    m_Zoomer->blockSignals(blocked);
}

void ControlBar::SetZoomLimits(float min, float max)
{
    m_Zoomer->setMinimum(min);
    m_Zoomer->setMaximum(max);
}

VOID_NAMESPACE_CLOSE
