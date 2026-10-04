// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QKeyEvent>

/* Internal */
#include "SNameEditor.h"

VOID_NAMESPACE_OPEN

SNameEditor::SNameEditor(QWidget* parent)
    : QLineEdit(parent)
{
    connect(this, &QLineEdit::returnPressed, this, [this]() -> void
    {
        emit accepted(text());
    });
}

void SNameEditor::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        emit cancelled();
    
    QLineEdit::keyPressEvent(event);
}

void SNameEditor::focusOutEvent(QFocusEvent* event)
{
    emit cancelled();
    QLineEdit::focusOutEvent(event);
}

VOID_NAMESPACE_CLOSE
