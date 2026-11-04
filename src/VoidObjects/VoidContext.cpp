// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "VoidContext.h"

VOID_NAMESPACE_OPEN

VoidContext& VoidContext::Instance()
{
    static VoidContext instance;
    return instance;
}

VOID_NAMESPACE_CLOSE
