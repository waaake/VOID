// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _Q_EXT_LINE_EDIT_H
#define _Q_EXT_LINE_EDIT_H

/* Qt */
#include <QLineEdit>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class IntBoundLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    IntBoundLineEdit(int lower, int upper, QWidget* parent = nullptr);
    IntBoundLineEdit(int lower, int upper, int value, QWidget* parent = nullptr);

    int CurrentValue() const { return text().toInt(); }

signals:
    void currentValueChanged(int value);

protected:
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private: /* Members */
    QString m_Last;

private:
    void Validate();
};

VOID_NAMESPACE_CLOSE

#endif // _Q_EXT_LINE_EDIT_H
