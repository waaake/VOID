// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _Q_EXT_COMBO_BOX_H
#define _Q_EXT_COMBO_BOX_H

/* Qt */
#include <QComboBox>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API ControlCombo : public QComboBox
{
public:
    ControlCombo(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

VOID_NAMESPACE_CLOSE

#endif // _Q_EXT_COMBO_BOX_H
