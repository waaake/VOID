// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _GRADE_OPERATOR_H
#define _GRADE_OPERATOR_H

/* Internal */
#include "Definition.h"
#include "Operator.h"

VOID_NAMESPACE_OPEN

class VOID_API GradeOp : public ImageOp
{
public:
    GradeOp();
    bool Evaluate(ImageRow& row) override;

private:
    Param* m_RedGain;
    Param* m_GreenGain;
    Param* m_BlueGain;
};

VOID_NAMESPACE_CLOSE

#endif // _GRADE_OPERATOR_H
