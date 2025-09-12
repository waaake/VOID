// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Globals.h"

VOID_NAMESPACE_OPEN

namespace UIGlobals {

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

MetadataViewer* GetMetadataViewer()
{
    if (g_VoidMainWindow)
        return g_VoidMainWindow->GetMetadataViewer();

    return nullptr;
}

} // namespace UIGlobals

VOID_NAMESPACE_CLOSE
