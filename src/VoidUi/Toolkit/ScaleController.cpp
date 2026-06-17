// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ScaleController.h"

VOID_NAMESPACE_OPEN

ScaleController::ScaleController(QWidget* parent)
    : ControlCombo(parent)
{
    Setup();
}

void ScaleController::SetScale(std::size_t scale)
{
    switch (scale)
    {
        case 1:
            setCurrentIndex(0);
            break;
        case 2:
            setCurrentIndex(1);
            break;
        case 4:
            setCurrentIndex(2);
            break;
        case 8:
            setCurrentIndex(3);
            break;
        default:
            setCurrentIndex(0);
    }
}

void ScaleController::Setup()
{
    addItems(
        {
            "1:1",
            "1:2",
            "1:4",
            "1:8"
        }
    );

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index) -> void
    {
        switch (index)
        {
            case 0:
                emit scaleChanged(1UL);
                break;
            case 1:
                emit scaleChanged(2UL);
                break;
            case 2:
                emit scaleChanged(4UL);
                break;
            case 3:
                emit scaleChanged(8UL);
                break;
        }
    });
}

VOID_NAMESPACE_CLOSE
