// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include "Effects.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

Effect::Effect(ImageOp* iop, const std::string& name, QObject* parent)
    : VoidObject(parent)
    , m_Operator(iop)
    , m_Name(name)
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

VOID_NAMESPACE_CLOSE
