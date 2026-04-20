// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Param.h"

VOID_NAMESPACE_OPEN

/* Param Value {{{ */

int ParamValue::GetInt() const
{
    if (auto v = Get<int>())
        return *v;
    return -1;
}

float ParamValue::GetFloat() const
{
    if (auto v = Get<float>())
        return *v;
    return 0.f;
}

bool ParamValue::GetBool() const
{
    if (auto v = Get<bool>())
        return *v;
    return false;
}

std::string ParamValue::GetString() const
{
    if (auto v = Get<std::string>())
        return *v;
    return "";
}

/* }}} */

/* Param {{{ */

int Param::GetInt() const
{
    if (auto v = value.Get<int>())
        return *v;
    return -1;
}

float Param::GetFloat() const
{
    if (auto v = value.Get<float>())
        return *v;
    return 0.f;
}

bool Param::GetBool() const
{
    if (auto v = value.Get<bool>())
        return *v;
    return false;
}

std::string Param::GetString() const
{
    if (auto v = value.Get<std::string>())
        return *v;
    return "";
}

/* }}} */

VOID_NAMESPACE_CLOSE
