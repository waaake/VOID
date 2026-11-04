// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cmath>
#include <string>

/* Internal */
#include "Globals.h"
#include "VoidBridge/Engine.h"

VOID_NAMESPACE_OPEN

namespace UIGlobals {

VoidMainWindow* g_VoidMainWindow = nullptr;
MenuSystem* g_MenuSystem = nullptr;

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
    return g_VoidMainWindow ? g_VoidMainWindow->ActivePlayer() : nullptr;
}

MetadataViewer* GetMetadataViewer()
{
    return g_VoidMainWindow ? g_VoidMainWindow->GetMetadataViewer() : nullptr;
}

SequencerTimeline* GetSequencer()
{
    return g_VoidMainWindow ? g_VoidMainWindow->GetSequencer() : nullptr;
}

void ToggleAudio(bool enable)
{
    _EngineBridge.ToggleAudio(enable);
}

void SetFramerate(float rate)
{
    _EngineBridge.SetFramerate(rate);
}

void SetLuminance(const QColor& color)
{
    /* Based on the Luminance, we set decide on the theme's darkness */
    double luminance = 0.2126 * color.redF() + 0.7152 * color.greenF() + 0.0722 * color.blueF();
    _EngineBridge.SetLuminance(luminance < 0.5);
}

void QueueTask(Task* task)
{
    if (g_VoidMainWindow)
        g_VoidMainWindow->QueueTask(task);
}

} // namespace UIGlobals

VOID_NAMESPACE_CLOSE
