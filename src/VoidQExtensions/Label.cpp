// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QMouseEvent>

/* Internal */
#include "Label.h"

VOID_NAMESPACE_OPEN

ClickableLabel::ClickableLabel(QWidget* parent)
    : QLabel(parent)
{
}

ClickableLabel::ClickableLabel(const QString& text, QWidget* parent)
    : QLabel(text, parent)
{
}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    /* Emit the clicked signal */
    emit clicked();
}

VOID_NAMESPACE_CLOSE
