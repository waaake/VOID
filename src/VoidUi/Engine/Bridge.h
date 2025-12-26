// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_ENGINE_BRIDGE_H
#define _VOID_ENGINE_BRIDGE_H

/* STD */
#include <string>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

namespace EngineBridge {

    VOID_API void OpenProject(const std::string& path);
    VOID_API void LoadMedia(const std::string& path);
    VOID_API void ClearIconCache();

} // namespace EngineBridge

VOID_NAMESPACE_CLOSE

#endif // _VOID_ENGINE_BRIDGE_H
