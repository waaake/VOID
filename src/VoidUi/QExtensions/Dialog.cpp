// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "Dialog.h"

VOID_NAMESPACE_OPEN

TranslucentDialog::TranslucentDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
}

void TranslucentDialog::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor background = palette().color(QPalette::Window);
    background.setAlpha(220);

    painter.setBrush(background);
    // painter.setPen(Qt::NoPen);
    painter.setPen(QPen(QBrush(palette().color(QPalette::Window).lighter(140)), 4));

    // painter.drawRoundedRect(rect(), 6, 6);
    painter.drawRect(rect());
}

VOID_NAMESPACE_CLOSE
