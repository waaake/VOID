// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_IMAGE_COMPARISON_RENDER_LAYER_H
#define _VOID_IMAGE_COMPARISON_RENDER_LAYER_H

/* Internal */
#include "Definition.h"
#include "PixReader.h"
#include "VoidRenderer/RenderTypes.h"
#include "VoidRenderer/Gears/ImageComparisonRenderGear.h"
#include "VoidRenderer/Gears/SwiperRenderGear.h"

VOID_NAMESPACE_OPEN

class ImageComparisonRenderLayer
{
public:
    ImageComparisonRenderLayer();
    ~ImageComparisonRenderLayer();

    /* Sets up the Render Components (Gears) */
    void Initialize();

    void Reset();

    void SetImageA(const SharedPixels& image);
    void SetImageB(const SharedPixels& image);

    /* Set Attributes for Render */
    inline void SetExposure(const float exposure) { m_ImageData->exposure = exposure; }
    inline void SetGamma(const float gamma) { m_ImageData->gamma = gamma; }
    inline void SetGain(const float gain) { m_ImageData->gain = gain; }

    inline void SetChannelMode(const int mode) { m_ImageData->channelMode = mode; }
    inline void SetChannelMode(const Renderer::ChannelMode& mode) { m_ImageData->channelMode = static_cast<int>(mode); }

    inline void SetComparisonMode(const int mode) { m_ImageData->comparisonMode = mode; }
    inline void SetComparisonMode(const Renderer::ComparisonMode& mode) { m_ImageData->comparisonMode = static_cast<int>(mode); }

    inline void SetBlendMode(const int mode) { m_ImageData->blendMode = mode; }
    inline void SetBlendMode(const Renderer::BlendMode& mode) { m_ImageData->blendMode = static_cast<int>(mode); }

    inline float SwipeX() const { return m_ImageData->swipeX; }
    inline void SetSwipeX(const float x) { m_ImageData->swipeX = x; }

    inline float SwipeOffset() const { return m_ImageData->offset; }
    inline void SetSwipeOffset(const float x) { m_ImageData->offset = x; }

    inline void ReinitShaderProgram() { m_ImageRenderer->Reinitialize(); }

    /* Main Render Function */
    void Render(const glm::mat4& projection);

private: /* Members */
    /* Render Data */
    Renderer::ImageComparisonRenderData* m_ImageData;

    /* Render Components */
    ImageComparisonRenderGear* m_ImageRenderer;
    SwiperRenderGear* m_SwiperRenderer;

    /* Render Texture */
    unsigned int m_TextureA;
    unsigned int m_TextureB;

    /* Internal Formats for GL */
    unsigned int m_InternalFormatA;
    unsigned int m_InternalFormatB;

private: /* Members */
    void InitTextureA(const SharedPixels& image);
    void InitTextureB(const SharedPixels& image);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_COMPARISON_RENDER_LAYER_H
