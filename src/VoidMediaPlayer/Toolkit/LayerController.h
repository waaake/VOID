// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _LAYER_CONTROLLER_H
#define _LAYER_CONTROLLER_H

/* STD */
#include <vector>

/* Internal */
#include "Definition.h"
#include "VoidQExtensions/ComboBox.h"

VOID_NAMESPACE_OPEN

class ImageLayerController : public ControlCombo
{
    Q_OBJECT
public:
    ImageLayerController(QWidget* parent = nullptr);

    void SetLayers(const std::vector<std::string>& layers);

signals:
    void layerChanged(const QString&);
};

VOID_NAMESPACE_CLOSE

#endif // _LAYER_CONTROLLER_H
