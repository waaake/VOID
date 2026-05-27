// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLabel>

/* Internal */
#include "ViewerController.h"
#include "GridController.h"
#include "VoidUi/Engine/IconForge.h"
#include "VoidUi/Player/PlayerBridge.h"

VOID_NAMESPACE_OPEN

ViewerController::ViewerController(QWidget* parent)
    : QWidget(parent)
    , m_ViewerControl(ViewerControl::None)
{
    Build();
    Setup();
    Connect();
}

ViewerController::~ViewerController()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void ViewerController::SetControl(const ViewerControl& control)
{
    m_ViewerControl = control;
    m_ControlOption->setVisible(control != ViewerControl::None);
    m_ControlOption->setIcon(Icon(control));
}

void ViewerController::Build()
{
    m_Layout = new QHBoxLayout(this);

    m_ControlOption = new QPushButton;
    m_Layout->addWidget(m_ControlOption);

    m_Layout->setContentsMargins(0, 0, 0, 0);
}

void ViewerController::Connect()
{
    connect(m_ControlOption, &QPushButton::clicked, this, [this]() -> void
    {
        if (m_ViewerControl == ViewerControl::GridControl)
        {
            GridController g;
            g.move(m_ControlOption->mapToGlobal(m_ControlOption->rect().bottomLeft()));
            g.exec();
        }
        else if (m_ViewerControl == ViewerControl::OnionSkinControl)
        {
            OnionSkinController osc;
            osc.move(m_ControlOption->mapToGlobal(m_ControlOption->rect().bottomLeft()));
            osc.exec();
        }
    });
}

void ViewerController::Setup()
{
    m_ControlOption->setFixedWidth(26);
    SetControl(ViewerControl::None);
}

QIcon ViewerController::Icon(const ViewerControl& control)
{
    switch (control)
    {
        case ViewerControl::OnionSkinControl: return IconForge::GetIcon(IconType::icon_trail_length_short, _DARK_COLOR(QPalette::Text, 100));
        case ViewerControl::GridControl:
        default: return IconForge::GetIcon(IconType::icon_grid_view, _DARK_COLOR(QPalette::Text, 100));
    }
}

/// OnionSkinController

OnionSkinController::OnionSkinController(QWidget* parent)
    : TranslucentDialog(parent)
{
    Build();
    Setup();

    connect(m_PeelSlider, &QSlider::valueChanged, this, [this](int value) -> void
    {
        _PlayerBridge.SetPeelFactor((float)value / 10.f);
    });
}

OnionSkinController::~OnionSkinController()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void OnionSkinController::Build()
{
    m_Layout = new QHBoxLayout(this);
    m_PeelSlider = new QSlider(Qt::Horizontal);

    m_Layout->addWidget(new QLabel("Peel Factor: ", this));
    m_Layout->addWidget(m_PeelSlider);
}

void OnionSkinController::Setup()
{
    m_PeelSlider->setRange(0, 10);
    m_PeelSlider->setValue(_PlayerBridge.PeelFactor() * 10);
}

VOID_NAMESPACE_CLOSE
