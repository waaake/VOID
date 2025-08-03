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

VOID_NAMESPACE_CLOSE
