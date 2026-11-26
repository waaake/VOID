// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _INVERT_OPERATOR_H
#define _INVERT_OPERATOR_H

/* Internal */
#include "Definition.h"
#include "Operator.h"

VOID_NAMESPACE_OPEN

class VOID_API InvertOp : public ImageRowOp
{
public:
    bool Evaluate(ImageRow& row) override;
    std::string Typename() const override { return "Invert"; }
};

VOID_NAMESPACE_CLOSE

#endif // _INVERT_OPERATOR_H
