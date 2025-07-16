#ifndef _VOID_IMAGE_RENDER_LAYER_H
#define _VOID_IMAGE_RENDER_LAYER_H

/* Internal */
#include "Definition.h"
#include "PixReader.h"
#include "VoidRenderer/RenderTypes.h"
#include "VoidRenderer/Gears/ImageRenderGear.h"

VOID_NAMESPACE_OPEN

class ImageRenderLayer
{
public:
    ImageRenderLayer();
    ~ImageRenderLayer();

    /* Sets up the Render Components (Gears) */
    void Initialize();

    void SetImage(const SharedPixels& image);

    /* Set Attributes for Render */
    inline void SetExposure(const float exposure) { m_ImageData->exposure = exposure; }
    inline void SetGamma(const float gamma) { m_ImageData->gamma = gamma; }
    inline void SetGain(const float gain) { m_ImageData->gain = gain; }

    inline void SetChannelMode(const int mode) { m_ImageData->channelMode = mode; }
    inline void SetChannelMode(const Renderer::ChannelMode& mode) { m_ImageData->channelMode = static_cast<int>(mode); }

    /* Main Render Function */
    void Render(const glm::mat4& projection);

private: /* Members */
    /* Render Data */
    Renderer::ImageRenderData* m_ImageData;

    /* Render Components */
    ImageRenderGear* m_ImageRenderer;

    /* Render Texture */
    unsigned int m_Texture;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_RENDER_LAYER_H
