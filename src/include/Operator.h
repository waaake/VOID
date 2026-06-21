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
#include "PixReader.h"

VOID_NAMESPACE_OPEN

class VOID_API ImageOp
{
public:
    virtual ~ImageOp();

    Param* GetParam(const std::string& name);
    virtual bool Evaluate(ImageRow& row) = 0;

    const std::vector<Param*>& Params() const { return m_Params; }

protected:
    Param* AddParam(std::string name, std::string label, ParamValue value, const Param::TypeDesc& type);
    Param* AddParam(std::string name, std::string label, ParamValue value, ParamRange range, const Param::TypeDesc& type);
    Param* AddParam(std::string name, std::string label, std::string description, ParamValue value, const Param::TypeDesc& type);
    Param* AddParam(std::string name, std::string label, std::string description, ParamValue value, ParamRange range, const Param::TypeDesc& type);

private:
    std::vector<Param*> m_Params;
};

typedef std::shared_ptr<ImageOp> SharedImageOp;

VOID_NAMESPACE_CLOSE

#endif // _BASE_OPERATOR_H
