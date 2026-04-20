// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Definition.h"
#include "Operator.h"

VOID_NAMESPACE_OPEN

Param* ImageOp::AddParam(const Param& param)
{
    auto [it, inserted] = m_Params.emplace(param.name, param);
    return inserted ? &it->second : nullptr;
}

Param* ImageOp::AddParam(Param&& param)
{
    auto [it, inserted] = m_Params.emplace(param.name, std::move(param));
    return inserted ? &it->second : nullptr;
}

Param* ImageOp::GetParam(const std::string& name)
{
    auto it = m_Params.find(name);
    return it == m_Params.end() ? nullptr : &it->second;
}

VOID_NAMESPACE_CLOSE
