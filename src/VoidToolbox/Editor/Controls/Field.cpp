// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Field.h"

VOID_NAMESPACE_OPEN

/// DoubleField

DoubleField::DoubleField(QWidget* parent)
    : DoubleField(0.0, 1000.0, 2, parent)
{
}

DoubleField::DoubleField(double lo, double hi, int decimals, QWidget* parent)
    : QLineEdit(parent)
    , m_Validator(new QDoubleValidator(this))
{
    m_Validator->setRange(lo, hi, decimals);
    setValidator(m_Validator);

    connect(this, &QLineEdit::editingFinished, this, [this]() -> void { emit valueChanged(Value()); });
    connect(this, &QLineEdit::returnPressed, this, [this]() -> void { emit valueChanged(Value()); });
}

DoubleField::~DoubleField()
{
    m_Validator->deleteLater();
    delete m_Validator;
    m_Validator = nullptr;
}

void DoubleField::SetValue(double v)
{
    setText(QString::number(v, 'f', m_Validator->decimals()));
}

double DoubleField::Value() const
{
    const QString& t = text();
    return t.isEmpty() ? 0.0 : t.toDouble();
}

void DoubleField::SetRange(double lo, double hi, int decimals)
{
    m_Validator->setRange(lo, hi, decimals);
}

/// IntField

IntField::IntField(QWidget* parent)
    : IntField(0, 1000, parent)
{
}

IntField::IntField(int lo, int hi, QWidget* parent)
    : QLineEdit(parent)
    , m_Validator(new QIntValidator(this))
{
    m_Validator->setRange(lo, hi);
    setValidator(m_Validator);
}

IntField::~IntField()
{
    m_Validator->deleteLater();
    delete m_Validator;
    m_Validator = nullptr;
}

void IntField::SetValue(int v)
{
    setText(QString::number(v));
}

int IntField::Value() const
{
    const QString& t = text();
    return t.isEmpty() ? 0 : t.toInt();
}

void IntField::SetRange(int lo, int hi)
{
    m_Validator->setRange(lo, hi);
}

VOID_NAMESPACE_CLOSE
