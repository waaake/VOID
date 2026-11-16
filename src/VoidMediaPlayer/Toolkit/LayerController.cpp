// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "LayerController.h"

VOID_NAMESPACE_OPEN

ImageLayerController::ImageLayerController(QWidget* parent)
    : ControlCombo(parent)
{
    setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(this, &QComboBox::currentTextChanged, this, &ImageLayerController::layerChanged);
}

void ImageLayerController::SetLayers(const std::vector<std::string>& layers)
{
    bool blocked = blockSignals(true);
    clear();
    for (const auto& layer : layers)
        addItem(layer.c_str());

    blockSignals(blocked);
}

VOID_NAMESPACE_CLOSE
