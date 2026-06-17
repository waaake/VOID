// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SCALE_CONTROLLER_H
#define _SCALE_CONTROLLER_H

/* Internal */
#include "Definition.h"
#include "ControlScroller.h"

VOID_NAMESPACE_OPEN

class ScaleController : public ControlCombo
{
    Q_OBJECT
public:
    ScaleController(QWidget* parent = nullptr);
    void SetScale(std::size_t scale);

signals:
    void scaleChanged(std::size_t);

private:
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _SCALE_CONTROLLER_H
