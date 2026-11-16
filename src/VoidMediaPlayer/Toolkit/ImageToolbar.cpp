// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLabel>

/* Internal */
#include "ImageToolbar.h"
#include "VoidIconForge/IconForge.h"
#include "VoidQExtensions/Frame.h"

VOID_NAMESPACE_OPEN

ImageToolbar::ImageToolbar(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Setup();
}

ImageToolbar::~ImageToolbar()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;

    m_LayerController->deleteLater();
    delete m_LayerController;
    m_LayerController = nullptr;
}

void ImageToolbar::Build()
{
    m_Layout = new QHBoxLayout(this);

    QLabel* stack = new QLabel(this);
    stack->setPixmap(IconForge::GetPixmap(IconType::icon_stacks, _DARK_COLOR(QPalette::Text, 100)));
    m_LayerController = new ImageLayerController(this);

    m_Layout->addWidget(stack);
    m_Layout->addWidget(m_LayerController);
    m_Layout->addWidget(new VLine(this));
    m_Layout->addStretch(1);
}

void ImageToolbar::Setup()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QMargins cm = m_Layout->contentsMargins();
    cm.setTop(4);
    cm.setBottom(4);
    m_Layout->setContentsMargins(cm);

    /// Signals
    connect(m_LayerController, &ImageLayerController::layerChanged, this, &ImageToolbar::imageLayerChangeRequested);
}

VOID_NAMESPACE_CLOSE
