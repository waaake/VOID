// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "ComboBox.h"

VOID_NAMESPACE_OPEN

/* Control Combo {{{ */

ControlCombo::ControlCombo(QWidget* parent)
    : QComboBox(parent)
{
    /* Set Focus to never stay on it */
    setFocusPolicy(Qt::NoFocus);
}

void ControlCombo::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(palette().text().color());
    painter.drawText(rect().adjusted(5, 0, -5, 0), Qt::AlignLeft | Qt::AlignVCenter, currentText());

    QPoint arrowCenter(width() - 15, height() * 0.5);
    QPolygon downarrow;
    downarrow << QPoint(arrowCenter.x() - 3, arrowCenter.y() + 1)
        << QPoint(arrowCenter.x() + 3, arrowCenter.y() + 1)
        << QPoint(arrowCenter.x(), arrowCenter.y() + 4);

    // Arrow polygon
    painter.setBrush(palette().text());
    painter.drawPolygon(downarrow);
}

VOID_NAMESPACE_CLOSE
