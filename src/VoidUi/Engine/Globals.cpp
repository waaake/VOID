// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cmath>
#include <string>

/* Internal */
#include "Globals.h"

VOID_NAMESPACE_OPEN

namespace UIGlobals {

VoidMainWindow* g_VoidMainWindow = nullptr;
MenuSystem* g_MenuSystem = nullptr;
static bool s_IsDarkTheme = false;
static float s_Framerate = 24.f;

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

void SetFramerate(float rate)
{
    s_Framerate = rate;
}

float Framerate()
{
    return s_Framerate;
}

std::string FramerateString()
{
    if (std::fabs(s_Framerate - std::round(s_Framerate)) < 1e-9)
        return std::to_string(static_cast<int>(s_Framerate));

    std::string fstr = std::to_string(s_Framerate);
    fstr.erase(fstr.find_last_not_of('0') + 1, std::string::npos);

    return fstr;
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
