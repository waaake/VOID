// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "SRulerWidget.h"
#include "VoidUi/Sequencer/SDescriptors.h"

VOID_NAMESPACE_OPEN

SRulerWidget::SRulerWidget(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(Sequencer::RulerHeight);
}

void SRulerWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.fillRect(rect(), palette().color(QPalette::AlternateBase));

    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(QRect(10, 0, width() - 20, height()), Qt::AlignVCenter, "Timeline");
}

VOID_NAMESPACE_CLOSE
