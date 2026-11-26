// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Internal */
#include "Definition.h"
#include "Operator.h"

VOID_NAMESPACE_OPEN

BaseOp::~BaseOp()
{
    for (auto& param : m_Params)
    {
        if (param)
        {
            delete param;
            param = nullptr;
        }
    }

    m_Params.clear();
}

Param* BaseOp::AddParam(std::string name, std::string label, ParamValue value, const Param::TypeDesc& type)
{
    // TODO: Change name and add param....
    if (GetParam(name))
        return nullptr;

    return m_Params.emplace_back(new Param(name, label, value, type));
}

Param* BaseOp::AddParam(std::string name, std::string label, ParamValue value, ParamRange range, const Param::TypeDesc& type)
{
    // TODO: Change name and add param....
    if (GetParam(name))
        return nullptr;

    return m_Params.emplace_back(new Param(name, label, value, range, type));
}

Param* BaseOp::AddParam(std::string name, std::string label, std::string description, ParamValue value, const Param::TypeDesc& type)
{
    // TODO: Change name and add param....
    if (GetParam(name))
        return nullptr;

    return m_Params.emplace_back(new Param(name, label, description, value, type));
}

Param* BaseOp::AddParam(std::string name, std::string label, std::string description, ParamValue value, ParamRange range, const Param::TypeDesc& type)
{
    // TODO: Change name and add param....
    if (GetParam(name))
        return nullptr;

    return m_Params.emplace_back(new Param(name, label, description, value, range, type));
}

Param* BaseOp::GetParam(std::string_view name)
{
    auto it = std::find_if(m_Params.begin(), m_Params.end(), [this, name](const Param* param) -> bool
    {
        return param->name == name;
    });

    return it == m_Params.end() ? nullptr : *it;
}

VOID_NAMESPACE_CLOSE
