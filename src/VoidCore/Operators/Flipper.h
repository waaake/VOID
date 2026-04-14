// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FLIP_OPERATOR_H
#define _FLIP_OPERATOR_H

/* Internal */
#include "Definition.h"
#include "Operator.h"

VOID_NAMESPACE_OPEN

class VOID_API FlipOp : public ImageOp
{
public:
    bool Evaluate(ImageRow& row) override;
};

VOID_NAMESPACE_CLOSE

#endif // _FLIP_OPERATOR_H
