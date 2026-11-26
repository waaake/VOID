// Copyright (c) 2025 waaake
// Licensed under the MIT License

#pragma once

/* Internal */
#include "Definition.h"
#include "FormatForge.h"
#include "Grader.h"
#include "Inverter.h"

VOID_NAMESPACE_OPEN

namespace Internal { 

    bool RegisterInvertOp()
    {
        OperatorRegistry r;
        r.name = "Invert";
        r.op = []() -> std::unique_ptr<BaseOp> { return std::make_unique<InvertOp>(); };

        return Forge::Instance().Register(r);
    }
    
    bool RegisterFlipOp()
    {
        return false;
    }
    
    bool RegisterColorGradeOp()
    {
        OperatorRegistry r;
        r.name = "ColorGrade";
        r.op = []() -> std::unique_ptr<BaseOp> { return std::make_unique<GradeOp>(); };
    
        return Forge::Instance().Register(r);
    }

    bool RegisterGrade2()
    {
        OperatorRegistry r;
        r.name = "Grade2";
        r.op = []() -> std::unique_ptr<BaseOp> { return std::make_unique<Grade2>(); };
    
        return Forge::Instance().Register(r);
    }

} // namespace Internal

void RegisterOperators()
{
    Internal::RegisterInvertOp();
    Internal::RegisterColorGradeOp();
    Internal::RegisterGrade2();
}

VOID_NAMESPACE_CLOSE
