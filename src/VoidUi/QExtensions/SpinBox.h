// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_Q_EXT_SPIN_BOX_H
#define _VOID_Q_EXT_SPIN_BOX_H

/* Qt */
#include <QDoubleSpinBox>

/* Internal */
#include "QDefinition.h"

VOID_NAMESPACE_OPEN

/**
 * SpinBox that allows click and drag to increment/decrement values quickly
 */
class ControlDoubleSpinner : public QDoubleSpinBox
{
public:
    ControlDoubleSpinner(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    /* Setup the Cursor Override */
    void enterEvent(EnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private: /* Members */
    /* Denotes the Last Mouse X Position */
    int m_LastX;

    /* Scroll threshold */
    int m_Threshold;

private: /* Methods */
    void Setup();
};


VOID_NAMESPACE_CLOSE

#endif // _VOID_Q_EXT_SPIN_BOX_H
