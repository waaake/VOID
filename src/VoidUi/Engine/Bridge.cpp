// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Bridge.h"
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
        _PlayerBridge.SetMedia(clip);
}

} // namespace EngineBridge

VOID_NAMESPACE_CLOSE
