// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Globals.h"

VOID_NAMESPACE_OPEN

namespace UIGlobals {

VoidMainWindow* g_VoidMainWindow = nullptr;
MenuSystem* g_MenuSystem = nullptr;
static bool s_IsDarkTheme = false;

VoidMainWindow* GetMainWindow()
{
    return g_VoidMainWindow;
}

MenuSystem* InternalMenuSystem()
{
    return g_MenuSystem;
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

void SetLuminance(const QColor& color)
{
    /* Based on the Luminance, we set decide on the theme's darkness */
    double luminance = 0.2126 * color.redF() + 0.7152 * color.greenF() + 0.0722 * color.blueF();
    s_IsDarkTheme = luminance < 0.5;
}

bool IsDarkTheme()
{
    return s_IsDarkTheme;
}

} // namespace UIGlobals

VOID_NAMESPACE_CLOSE
