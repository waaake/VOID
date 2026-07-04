// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PLAYER_COMPONENTS_H
#define _PLAYER_COMPONENTS_H

/* Internal */
#include "Definition.h"
#include "Image.h"
#include "VoidRenderer/Core/RenderTypes.h"

VOID_NAMESPACE_OPEN

/**
 * Enum decribing which viewer buffer is currently active and can be used to set an
 * active viewer buffer for the
 */
enum class VOID_API PlayerViewBuffer
{
    /* Relates to ViewerBuffer A */
    A,
    /* Relates to ViewerBuffer B */
    B

    /* In Future the Buffer type may also include Compare buffers like switch/swipe and others. */
};

struct BufferData
{
    FloatImage image = nullptr;
    Renderer::SharedAnnotation annotation = nullptr;

    bool Valid() const { return (bool)image; }
    explicit operator bool() const { return (bool)image; }
};

VOID_NAMESPACE_CLOSE

#endif // _PLAYER_COMPONENTS_H
