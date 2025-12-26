// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Bridge.h"
#include "VoidUi/Engine/IconForge.h"
#include "VoidUi/Player/PlayerBridge.h"
#include "VoidUi/Project/ProjectBridge.h"

VOID_NAMESPACE_OPEN

namespace EngineBridge {

void OpenProject(const std::string& path)
{
    _ProjectBridge.Open(path);
}

void LoadMedia(const std::string& path)
{
    if (const SharedMediaClip& clip = _ProjectBridge.OpenMedia(path))
    {
        /**
         * TODO: The player currently faces a race condition when caching media and playing
         * the first frame, there's mostly a case where the frame is read but not cached completely
         * at the moment, calling refresh (re-render the current frame) works to load the first frame
         * of the media, but this needs some change, possibly on the way how media cache is handled 
         */
        _PlayerBridge.SetMedia(clip);
        _PlayerBridge.Refresh();
    }
}

void ClearIconCache()
{
    IconForge::Instance().ClearCache();
}

} // namespace EngineBridge

VOID_NAMESPACE_CLOSE
