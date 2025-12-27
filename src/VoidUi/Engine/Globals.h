// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_UI_GLOBALS_H
#define _VOID_UI_GLOBALS_H

/* Qt */
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/MetadataViewer.h"
#include "VoidUi/BaseWindow/MenuSystem.h"
#include "VoidUi/BaseWindow/PlayerWindow.h"

VOID_NAMESPACE_OPEN

namespace UIGlobals {

    /* Tracks the MainWindow */
    extern VOID_API VoidMainWindow* g_VoidMainWindow;
    extern VOID_API MenuSystem* g_MenuSystem;

    VOID_API VoidMainWindow* GetMainWindow();
    VOID_API MenuSystem* InternalMenuSystem();
    VOID_API Player* GetActivePlayer();
    VOID_API MetadataViewer* GetMetadataViewer();
    VOID_API void SetLuminance(const QColor& color);
    VOID_API bool IsDarkTheme();

} // namespace Globals

VOID_NAMESPACE_CLOSE

#endif // _VOID_UI_GLOBALS_H
