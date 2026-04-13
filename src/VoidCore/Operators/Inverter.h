// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _INVERT_OPERATOR_H
#define _INVERT_OPERATOR_H

/* Internal */
#include "Definition.h"
#include "Operator.h"

VOID_NAMESPACE_OPEN

class VOID_API InvertOp : public ImageOp
{
public:
    bool Evaluate(const SharedPixels& image) override;
};

VOID_NAMESPACE_CLOSE

#endif // _INVERT_OPERATOR_H
