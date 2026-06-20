// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Definition.h"
#include "Operator.h"

VOID_NAMESPACE_OPEN

ImageOp::~ImageOp()
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

Param* ImageOp::AddParam(std::string name, std::string label, ParamValue value, const Param::TypeDesc& type)
{
    // TODO: Change name and add param....
    if (GetParam(name))
        return nullptr;

    return m_Params.emplace_back(new Param(name, label, value, type));
}

Param* ImageOp::AddParam(std::string name, std::string label, ParamValue value, ParamRange range, const Param::TypeDesc& type)
{
    // TODO: Change name and add param....
    if (GetParam(name))
        return nullptr;

    return m_Params.emplace_back(new Param(name, label, value, range, type));
}

Param* ImageOp::AddParam(std::string name, std::string label, std::string description, ParamValue value, const Param::TypeDesc& type)
{
    // TODO: Change name and add param....
    if (GetParam(name))
        return nullptr;

    return m_Params.emplace_back(new Param(name, label, description, value, type));
}

Param* ImageOp::AddParam(std::string name, std::string label, std::string description, ParamValue value, ParamRange range, const Param::TypeDesc& type)
{
    // TODO: Change name and add param....
    if (GetParam(name))
        return nullptr;

    return m_Params.emplace_back(new Param(name, label, description, value, range, type));
}

Param* ImageOp::GetParam(const std::string& name)
{
    for (auto& param : m_Params)
    {
        if (param->name == name)
            return param;
    }

    return nullptr;
}

VOID_NAMESPACE_CLOSE
