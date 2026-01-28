// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_IMAGE_RENDER_LAYER_H
#define _VOID_IMAGE_RENDER_LAYER_H

/* Internal */
#include "Definition.h"
#include "PixReader.h"
#include "VoidRenderer/Core/RenderTypes.h"
#include "VoidRenderer/Programs/ImageShaderProgram.h"

VOID_NAMESPACE_OPEN

class ImageRenderLayer
{
public:
    ImageRenderLayer();
    ~ImageRenderLayer();

    /* Sets up the Render Components (Gears) */
    void Initialize();

    void Reset();
    void SetImage(const SharedPixels& image);

    /* Set Attributes for Render */
    inline void SetExposure(const float exposure) { m_Exposure = exposure; }
    inline void SetGamma(const float gamma) { m_Gamma = gamma; }
    inline void SetGain(const float gain) { m_Gain = gain; }

    inline void SetChannelMode(const int mode) { m_ChannelMode = mode; }
    inline void SetChannelMode(const Renderer::ChannelMode& mode) { m_ChannelMode = static_cast<int>(mode); }

    /**
     * @brief Reinitializes the shaders and internals (Vertex Array Objects | Vertex Buffer Objects | Element/Index Buffer Objects)
     * 
     */
    void ReinitShaderProgram();

    /* Main Render Function */
    void Render(const glm::mat4& projection);

private: /* Members */
    /* Projection for the Texture for the viewport */
    glm::mat4 m_Projection;

    /* Render Attributes affecting how the image is displayed */
    float m_Exposure;
    float m_Gamma;
    float m_Gain;
    int m_ChannelMode;
    int m_InputColorSpace;

    /* Render Components */
    ImageShaderProgram* m_Shader;

    /**
     * Array and Buffer objects
     *
     * Vertex array Object
     * Vertex Buffer Object
     * Element or the index buffer object
     */
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_IBO;
    unsigned int m_PBOs[2];
    int m_PBOIndex;

    /* Uniforms */
    int m_UProjection;
    int m_UTexture;
    int m_UExposure;
    int m_UGamma;
    int m_UGain;
    int m_UChannelMode;
    int m_UInputColorSpace;

    /* Render Texture */
    unsigned int m_Texture;
    unsigned int m_InternalFormat;

private: /* Methods */
    /**
     * @brief Reinitializes and re-allocates the buffer data based on the image size.
     * 
     * @param image The image data to be loaded.
     */
    void ReinitBuffer(const SharedPixels& image);

    /**
     * @brief Setup Array Buffers
     * Initialize the Array Buffers to be used in the program
     * 
     */
    void SetupBuffers();

    /**
     * @brief Pre-Draw Call
     * Setup anything which is required before drawing anything on the screen
     * 
     */
    bool PreDraw();

    /**
     * @brief The Main Draw Call
     * This is invoked if the PreDraw is successful
     *
     */
    void Draw();

    /**
     * @brief The Post Draw Call
     * Anything to be cleaned up after the draw is completed can be done here
     * This method always gets called even if the draw isn't called this will be
     * 
     */
    void PostDraw();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_RENDER_LAYER_H
