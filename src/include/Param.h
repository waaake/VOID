// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PARAMETERS_H
#define _PARAMETERS_H

/* STD */
#include <string>
#include <variant>
#include <optional>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class ParamValue
{
public:
    ParamValue() = default;
    ParamValue(int v) : data(v) {}
    ParamValue(float v) : data(v) {}
    ParamValue(bool v) : data(v) {}
    ParamValue(std::string v) : data(std::move(v)) {}

    template <typename _Ty>
    void Set(_Ty v) { data = v; }

    template <typename _Ty>
    bool IsType() const { return std::holds_alternative<_Ty>(data); }

    template <typename _Ty>
    std::optional<_Ty> Get() const
    {
        if (std::holds_alternative<_Ty>(data))
            return std::get<_Ty>(data);
        return std::nullopt;
    }

    int GetInt() const;
    float GetFloat() const;
    bool GetBool() const;
    std::string GetString() const;

private:
    std::variant<bool, int, float, std::string> data;
};

struct Param
{
    enum class TypeDesc { Int, Float, Boolean, String };

    std::string name;
    TypeDesc type;
    ParamValue value;

    Param()
        : name(""), type(TypeDesc::Int), value(0) {}

    Param(std::string name, ParamValue value, const TypeDesc& type)
        : name(std::move(name)), type(type), value(std::move(value)) {}

    template <typename _Ty>
    void SetValue(_Ty v) { value.Set(v); }

    // Helpers
    void SetInt(int v) { value.Set(v); }
    void SetFloat(float v) { value.Set(v); }
    void SetBool(bool v) { value.Set(v); }
    void SetString(const std::string& v) { value.Set(v); }

    int GetInt() const;
    float GetFloat() const;
    bool GetBool() const;
    std::string GetString() const;
};

VOID_NAMESPACE_CLOSE

#endif // _PARAMETERS_H
