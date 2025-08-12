// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "VoidGlobals.h"

VOID_NAMESPACE_OPEN

VoidMainWindow* g_VoidMainWindow = nullptr;

VoidMainWindow* GetMainWindow()
{
    return g_VoidMainWindow;
}

Player* GetActivePlayer()
{
    if (g_VoidMainWindow)
        return g_VoidMainWindow->ActivePlayer();

    return nullptr;
}

VOID_NAMESPACE_CLOSE
