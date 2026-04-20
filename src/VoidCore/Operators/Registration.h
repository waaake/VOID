// Copyright (c) 2025 waaake
// Licensed under the MIT License

#pragma once

/* Internal */
#include "Definition.h"
#include "FormatForge.h"
#include "Grader.h"

VOID_NAMESPACE_OPEN

namespace Internal { 

    bool RegisterInvertOp()
    {
        return false;
    }
    
    bool RegisterFlipOp()
    {
        return false;
    }
    
    bool RegisterColorGradeOp()
    {
        IOpRegistry r;
        r.name = "ColorGrade";
        r.iop = []() -> std::unique_ptr<ImageOp> { return std::make_unique<GradeOp>(); };
    
        return Forge::Instance().Register(r);
    }

} // namespace Internal

void RegisterOperators()
{
    Internal::RegisterColorGradeOp();
}

VOID_NAMESPACE_CLOSE
