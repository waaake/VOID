// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_COMPONENT_VIEW_H
#define _VOID_COMPONENT_VIEW_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class IView
{
public:
    virtual ~IView() = default;
    virtual void DeleteSelected() = 0;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_COMPONENT_VIEW_H
