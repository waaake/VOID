// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _BASE_OPERATOR_H
#define _BASE_OPERATOR_H

/* STD */
#include <memory>
#include <map>

/* Internal */
#include "Definition.h"
#include "Param.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

class VOID_API ImageOp
{
public:
    virtual ~ImageOp() = default;

    Param* GetParam(const std::string& name);    
    virtual bool Evaluate(ImageRow& row) = 0;

    const std::map<std::string, Param>& Params() const { return m_Params; }

protected:
    Param* AddParam(const Param& param);
    Param* AddParam(Param&& param);

private:
    std::map<std::string, Param> m_Params;
};

typedef std::shared_ptr<ImageOp> SharedImageOp;

VOID_NAMESPACE_CLOSE

#endif // _BASE_OPERATOR_H
