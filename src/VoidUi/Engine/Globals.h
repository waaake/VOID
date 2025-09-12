// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_UI_GLOBALS_H
#define _VOID_UI_GLOBALS_H

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/MetadataViewer.h"
#include "VoidUi/BaseWindow/PlayerWindow.h"

VOID_NAMESPACE_OPEN

namespace UIGlobals {

    /* Tracks the MainWindow */
    extern VOID_API VoidMainWindow* g_VoidMainWindow;

    VOID_API VoidMainWindow* GetMainWindow();
    VOID_API Player* GetActivePlayer();
    VOID_API MetadataViewer* GetMetadataViewer();

} // namespace Globals

VOID_NAMESPACE_CLOSE

#endif // _VOID_UI_GLOBALS_H
