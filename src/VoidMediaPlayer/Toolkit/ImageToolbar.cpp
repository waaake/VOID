// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLabel>

/* Internal */
#include "ImageToolbar.h"
#include "VoidIconForge/IconForge.h"
#include "VoidQExtensions/Frame.h"
#include "VoidQExtensions/Tooltip.h"

VOID_NAMESPACE_OPEN

ImageToolbar::ImageToolbar(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Setup();
}

ImageToolbar::~ImageToolbar()
{
    m_LayerController->deleteLater();
    delete m_LayerController;
    m_LayerController = nullptr;

    m_ExposureSpinner->deleteLater();
    delete m_ExposureSpinner;
    m_ExposureSpinner = nullptr;

    m_GainSpinner->deleteLater();
    delete m_GainSpinner;
    m_GainSpinner = nullptr;

    m_GammaSpinner->deleteLater();
    delete m_GammaSpinner;
    m_GammaSpinner = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void ImageToolbar::Build()
{
    m_Layout = new QHBoxLayout(this);
    m_LayerController = new ImageLayerController(this);

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
    
    m_Layout->addWidget(m_ExposureSpinner);
    m_Layout->addWidget(new VLine(this));
    m_Layout->addWidget(m_GammaSpinner);
    m_Layout->addWidget(new VLine(this));
    m_Layout->addWidget(m_GainSpinner);
    m_Layout->addWidget(new VLine(this));
    
    m_Layout->addStretch(1);

    m_Layout->addWidget(new VLine(this));
    m_Layout->addWidget(m_LayerController);
}

void ImageToolbar::Setup()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QMargins cm = m_Layout->contentsMargins();
    cm.setTop(4);
    cm.setBottom(4);
    m_Layout->setContentsMargins(cm);

    m_ExposureSpinner->SetDefault(0.f);
    m_ExposureSpinner->SetRange(-5.f, 5.f);
    m_ExposureSpinner->SetStep(0.1f);
    m_ExposureSpinner->SetLabel("E");
    m_ExposureSpinner->SetValue(0.f);

    m_GammaSpinner->SetDefault(1.0);
    m_GammaSpinner->SetRange(1.f, 3.f);
    m_GammaSpinner->SetStep(0.1f);
    m_GammaSpinner->SetLabel(QStringLiteral("γ").toStdString().c_str());

    m_GammaSpinner->SetValue(1.0);

    m_GainSpinner->SetDefault(1.f);
    m_GainSpinner->SetRange(0.5f, 3.5f);
    m_GainSpinner->SetStep(0.1f);
    m_GainSpinner->SetLabel("f/");
    m_GainSpinner->SetValue(1.f);

    /// Signals
    connect(m_ExposureSpinner, &QuickSpinner::valueChanged, this, &ImageToolbar::exposureChanged);
    connect(m_GammaSpinner, &QuickSpinner::valueChanged, this, &ImageToolbar::gammaChanged);
    connect(m_GainSpinner, &QuickSpinner::valueChanged, this, &ImageToolbar::gainChanged);
    connect(m_LayerController, &ImageLayerController::layerChanged, this, &ImageToolbar::imageLayerChangeRequested);
}

VOID_NAMESPACE_CLOSE
