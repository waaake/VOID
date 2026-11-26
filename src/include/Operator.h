// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _BASE_OPERATOR_H
#define _BASE_OPERATOR_H

/* STD */
#include <memory>
#include <vector>

/* Internal */
#include "Definition.h"
#include "Param.h"
#include "Image.h"

VOID_NAMESPACE_OPEN

/// @brief Describes core Operator types, based on which the eval type is decided
enum class OpType
{
    NoOp,
    ImageOp,
    RowOp
};

/// @brief Very base of operations, provides basic functionality around a node operator
/// Like the name, type and a way to store the internal parameters
/// Also ways to describe the core type of the operator and the Typename i.e. type of the implemented Operator e.g. Grade or Transform
class VOID_API BaseOp
{
public:
    virtual ~BaseOp();

    Param* GetParam(std::string_view name);
    virtual std::string Typename() const = 0;
    virtual v_frame_t Frame(v_frame_t in) { return in; }

    const std::vector<Param*>& Params() const { return m_Params; }
    virtual OpType OperatorType() const = 0;

protected:
    Param* AddParam(std::string name, std::string label, ParamValue value, const Param::TypeDesc& type);
    Param* AddParam(std::string name, std::string label, ParamValue value, ParamRange range, const Param::TypeDesc& type);
    Param* AddParam(std::string name, std::string label, std::string description, ParamValue value, const Param::TypeDesc& type);
    Param* AddParam(std::string name, std::string label, std::string description, ParamValue value, ParamRange range, const Param::TypeDesc& type);

private:
    std::vector<Param*> m_Params;
};

/// @brief ImageRowOp is an operator type that allows the image to be processed in rows i.e. scanlines
/// the internal implementation takes care of splicing the image into rows that may or may not be processed in parallel
/// the implementing operator takes care of handling the processing of all the channels in a row of image pixels
class VOID_API ImageRowOp : public BaseOp
{
public:
    virtual bool Evaluate(ImageRow& row) = 0;
    OpType OperatorType() const { return OpType::RowOp; }
};

/// @brief ImageOp is a type that provides access to the full image data at once and the implementing operator can then
/// perform operation on the entire image at once, any sort of parallel processing or tiling can be handled at the Operator level.
/// Certain operations like transforming the image, like shear, scale or crop utilise this operator type
class VOID_API ImageOp : public BaseOp
{
public:
    virtual bool Evaluate(FloatImage& image) = 0;
    OpType OperatorType() const { return OpType::ImageOp; }
};

/// @brief Acts as an Operator which does not evaluate or process anything on the image, more for storing or acting upon some metadata
class VOID_API NoOp : public BaseOp
{
public:
    OpType OperatorType() const { return OpType::NoOp; }
};

typedef std::shared_ptr<ImageOp> SharedImageOp;

VOID_NAMESPACE_CLOSE

#endif // _BASE_OPERATOR_H
