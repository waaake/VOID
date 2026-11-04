// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_DESCRIPTORS_H
#define _MEDIA_DESCRIPTORS_H

/* Internal */
#include "Definition.h"
#include "Audio.h"
#include "Image.h"
#include "VoidRenderer/Core/RenderTypes.h"

VOID_NAMESPACE_OPEN

namespace MediaDesc {

struct RenderBuffer
{
    FloatImage image = nullptr;
    SharedFrameAudio audio = nullptr;
    Renderer::SharedAnnotation annotation = nullptr;
};

} // namespace MediaDesc

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_DESCRIPTORS_H
