// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _BRIGHTEN_OPERATOR_H
#define _BRIGHTEN_OPERATOR_H

/* Internal */
#include "Definition.h"
#include "Operator.h"

VOID_NAMESPACE_OPEN

class VOID_API BrightenOp : public ImageOp
{
public:
    BrightenOp();
    bool Evaluate(ImageRow& row) override;
private:
    Param* m_Gain;
};

VOID_NAMESPACE_CLOSE

#endif // _BRIGHTEN_OPERATOR_H
