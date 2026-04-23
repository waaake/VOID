// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include "Effects.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

Effect::Effect(ImageOp* iop, const std::string& name, QObject* parent)
    : VoidObject(parent)
    , m_Operator(iop)
    , m_Name(name)
    , m_Enabled(true)
{
}

Effect::~Effect()
{
    if (m_Operator)
    {
        delete m_Operator;
        m_Operator = nullptr;
    }
}

bool Effect::SetValue(const std::string& param, ValueType value)
{
    if (Param* p = m_Operator->GetParam(param))
    {
        p->SetValue(value);

        emit updated();
        emit valueChanged(p);
        return true;
    }

    return false;
}

void Effect::ResetValue(const std::string& param)
{
    if (Param* p = m_Operator->GetParam(param))
    {
        p->ResetValue();

        emit updated();
        emit valueChanged(p);
    }
}

const ValueType& Effect::Value(const std::string& param) const
{
    if (Param* p = m_Operator->GetParam(param))
        return p->Value();

    static ValueType invalid = std::monostate{};
    return invalid;
}

void Effect::SetEnabled(bool enable)
{
    m_Enabled = enable;
    emit updated();
}

VOID_NAMESPACE_CLOSE
