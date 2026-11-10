// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ColorController.h"
#include "VoidCore/Processors/ColorProcessor.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

ColorController::ColorController(QWidget* parent)
    : ControlCombo(parent)
{
    Setup();
    connect(this, &QComboBox::currentTextChanged, this, [this](const QString& value) { emit colorDisplayChanged(value.toStdString());});
}

void ColorController::Setup()
{
    ColorProcessor& proc = ColorProcessor::Instance();
    for (std::string& display : proc.Displays())
        addItem(display.c_str());

    setCurrentText(proc.DefaultDisplay().c_str());
}

VOID_NAMESPACE_CLOSE
