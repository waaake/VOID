// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_UI_GLOBALS_H
#define _VOID_UI_GLOBALS_H

/* Qt */
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidEngine/BaseWindow/PlayerWindow.h"
#include "VoidObjects/Core/Task.h"
#include "VoidMediaPlayer/Media/MetadataViewer.h"
#include "VoidMenuSystem/MenuSystem.h"
#include "VoidSequencer/Sequencer.h"

VOID_NAMESPACE_OPEN

namespace UIGlobals {

    /* Tracks the MainWindow */
    extern VOID_API VoidMainWindow* g_VoidMainWindow;
    extern VOID_API MenuSystem* g_MenuSystem;

    VOID_API VoidMainWindow* GetMainWindow();
    VOID_API MenuSystem* InternalMenuSystem();
    VOID_API Player* GetActivePlayer();
    VOID_API MetadataViewer* GetMetadataViewer();
    VOID_API SequencerTimeline* GetSequencer();
    VOID_API void ToggleAudio(bool enable);
    VOID_API void SetFramerate(float rate);
    VOID_API void SetLuminance(const QColor& color);
    VOID_API bool IsDarkTheme();
    VOID_API void QueueTask(Task* task);

} // namespace Globals

VOID_NAMESPACE_CLOSE

#endif // _VOID_UI_GLOBALS_H
