// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "RenderGear.h"

VOID_NAMESPACE_OPEN

void RenderGear::Render(const void* data)
{
    /* Pre Draw */
    if (PreDraw())
    {
        /* Draw with the data */
        Draw(data);
    }

    /* Post Draw Process */
    PostDraw();
}

void RenderGear::ReallocatePixelBuffer(std::size_t, const PixelBuffer&)
{
}

void RenderGear::RebindPixelBuffer(const PixelBuffer&)
{
}

void RenderGear::WritePixelData(const void*, std::size_t)
{
}

void RenderGear::UnbindPixelBuffer()
{
}

VOID_NAMESPACE_CLOSE
