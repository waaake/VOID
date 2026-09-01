// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_COMPONENT_H
#define _VOID_COMPONENT_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class IComponent
{
public:
    virtual ~IComponent() = default;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_COMPONENT_H
