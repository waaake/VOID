// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PARAMETERS_H
#define _PARAMETERS_H

/* STD */
#include <string>
#include <optional>
#include <variant>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

typedef std::variant<bool, int, float, std::string, std::monostate> ValueType;

class VOID_API ParamValue
{
public:
    ParamValue() = default;
    ParamValue(int v) : data(v) {}
    ParamValue(float v) : data(v) {}
    ParamValue(bool v) : data(v) {}
    ParamValue(std::string v) : data(std::move(v)) {}
    ParamValue(ValueType v) : data(std::move(v)) {}

    template <typename _Ty>
    void Set(_Ty v) { data = v; }
    void SetValue(ValueType v) { data = v; }

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

    const ValueType& Value() const { return data; }

private:
    ValueType data;
};

/**
 * @brief Defines the working range of the parameter
 * Applicable only for the int and float types, this defines the upper and lower
 * bounds on the param editor allowing users to play with the values
 * 
 */
struct VOID_API ParamRange
{
    float lower;
    float upper;
    float step;

    ParamRange() : ParamRange(0, 0, 0.1f) {}
    /**
     * @brief Construct a new Param Range
     * 
     * @param lower lower bound.
     * @param upper upper bound.
     */
    ParamRange(float lower, float upper) : ParamRange(lower, upper, 0.1f) {}
    /**
     * @brief Construct a new Param Range.
     * 
     * @param lower lower bound.
     * @param upper upper bound.
     * @param step defines how much should change with each delta (increment/decrement).
     */
    ParamRange(float lower, float upper, float step) : lower(lower), upper(upper), step(step) {}

    inline explicit operator bool() const noexcept { return lower != upper; }
};

struct VOID_API Param
{
    enum class TypeDesc { Int, Float, Boolean, String };

    std::string name;
    std::string label;
    std::string description;
    TypeDesc type;
    ParamValue value;
    ParamValue preset;
    ParamRange range;

    Param()
        : name(""), label(""), description(""), type(TypeDesc::Int), value(0), preset(0) {}

    Param(std::string name, std::string label, ParamValue value, const TypeDesc& type)
        : name(std::move(name)), label(std::move(label)), description("")
        , type(type), value(value), preset(std::move(value)) {}

    Param(std::string name, std::string label, ParamValue value, ParamRange range, const TypeDesc& type)
        : name(std::move(name)), label(std::move(label)), description("")
        , type(type), value(value), preset(std::move(value)), range(std::move(range)) {}

    Param(std::string name, std::string label, std::string description, ParamValue value, const TypeDesc& type)
        : name(std::move(name)), label(std::move(label)), description(std::move(description))
        , type(type), value(value), preset(std::move(value)) {}

    Param(std::string name, std::string label, std::string description, ParamValue value, ParamRange range, const TypeDesc& type)
        : name(std::move(name)), label(std::move(label)), description(std::move(description))
        , type(type), value(value), preset(std::move(value)), range(std::move(range)) {}

    template <typename _Ty>
    void SetValue(_Ty v) { value.Set(v); }
    void SetValue(ValueType v) { value.SetValue(v); }
    void ResetValue() { value = preset; }
    const ValueType& Value() const { return value.Value(); }

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
