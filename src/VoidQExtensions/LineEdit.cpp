// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QIntValidator>
#include <QKeyEvent>

/* Internal */
#include "LineEdit.h"

VOID_NAMESPACE_OPEN

IntBoundLineEdit::IntBoundLineEdit(int lower, int upper, QWidget* parent)
    : QLineEdit(parent)
    , m_Last(QString::number(upper))
{
    setValidator(new QIntValidator(lower, upper, this));
}

IntBoundLineEdit::IntBoundLineEdit(int lower, int upper, int value, QWidget* parent)
    : QLineEdit(parent)
    , m_Last(QString::number(value))
{
    setText(m_Last);
    setValidator(new QIntValidator(lower, upper, this));   
}

void IntBoundLineEdit::focusOutEvent(QFocusEvent* event)
{
    QLineEdit::focusOutEvent(event);
    Validate();
}

void IntBoundLineEdit::keyPressEvent(QKeyEvent* event)
{
    QLineEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        Validate();
}

void IntBoundLineEdit::Validate()
{
    QString t = text();

    if (t == m_Last)
        return;

    int pos = 0;
    if (validator()->validate(t, pos) == QIntValidator::Acceptable)
    {
        m_Last = t;
        emit currentValueChanged(t.toInt());
    }
    else
    {
        setText(m_Last);
    }
}

VOID_NAMESPACE_CLOSE
