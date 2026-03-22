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
    background.setAlpha(210);

    painter.setBrush(background);
    painter.setPen(Qt::NoPen);

    // painter.drawRoundedRect(rect(), 10, 10);
    painter.drawRect(rect());
}

VOID_NAMESPACE_CLOSE
