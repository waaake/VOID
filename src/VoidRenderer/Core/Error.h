// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _RENDERER_ERROR_LOG_H
#define _RENDERER_ERROR_LOG_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

namespace Renderer {

void GLGetError();
void GLGetError(const std::string& base);

} // namespace Renderer

VOID_NAMESPACE_CLOSE

#endif // _RENDERER_ERROR_LOG_H
