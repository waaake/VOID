// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _EDITOR_CONTROL_FIELD_H
#define _EDITOR_CONTROL_FIELD_H

/* Qt */
#include <QDoubleValidator>
#include <QIntValidator>
#include <QLineEdit>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class DoubleField : public QLineEdit
{
    Q_OBJECT
public:
    explicit DoubleField(QWidget* parent = nullptr);
    DoubleField(double lo, double hi, int decimals = 0, QWidget* parent = nullptr);
    ~DoubleField();

    void SetValue(double v);
    double Value() const;
    void SetRange(double lo, double hi, int decimals = 0);

signals:
    void valueChanged(double v);

private:
    QDoubleValidator* m_Validator;
};

class IntField : public QLineEdit
{
    Q_OBJECT
public:
    explicit IntField(QWidget* parent = nullptr);
    IntField(int lo, int hi, QWidget* parent = nullptr);
    ~IntField();

    void SetValue(int v);
    int Value() const;
    void SetRange(int hi, int lo);

signals:
    void valueChanged(int v);

private:
    QIntValidator* m_Validator;
};

VOID_NAMESPACE_CLOSE

#endif // _EDITOR_CONTROL_FIELD_H
