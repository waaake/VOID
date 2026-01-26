// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_IMAGE_COMPARISON_RENDER_LAYER_H
#define _VOID_IMAGE_COMPARISON_RENDER_LAYER_H

/* Internal */
#include "Definition.h"
#include "PixReader.h"
#include "VoidRenderer/RenderTypes.h"
#include "VoidRenderer/Programs/ImageComparisonShaderProgram.h"
#include "VoidRenderer/Programs/SwiperShaderProgram.h"

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
    inline void SetExposure(const float exposure) { m_Exposure = exposure; }
    inline void SetGamma(const float gamma) { m_Gamma = gamma; }
    inline void SetGain(const float gain) { m_Gain = gain; }

    inline void SetChannelMode(const int mode) { m_ChannelMode = mode; }
    inline void SetChannelMode(const Renderer::ChannelMode& mode) { m_ChannelMode = static_cast<int>(mode); }

    inline void SetComparisonMode(const int mode) { m_ComparisonMode = mode; }
    inline void SetComparisonMode(const Renderer::ComparisonMode& mode) { m_ComparisonMode = static_cast<int>(mode); }

    inline void SetBlendMode(const int mode) { m_BlendMode = mode; }
    inline void SetBlendMode(const Renderer::BlendMode& mode) { m_BlendMode = static_cast<int>(mode); }

    inline float SwipeX() const { return m_SwipeX; }
    inline void SetSwipeX(const float x) { m_SwipeX = x; }

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
    int m_ComparisonMode;
    int m_BlendMode;
    int m_InputColorSpaceA;
    int m_InputColorSpaceB;

    float m_SwipeX;

    /* Render Components */
    ImageComparisonShaderProgram* m_Shader;

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
    unsigned int m_PBOs_A[2];
    unsigned int m_PBOs_B[2];
    int m_PBOIndexA;
    int m_PBOIndexB;

    /* Uniforms */
    int m_UProjection;
    int m_UTextureA;
    int m_UTextureB;
    int m_UExposure;
    int m_UGamma;
    int m_UGain;
    int m_UChannelMode;
    int m_UComparisonMode;
    int m_UBlendMode;
    int m_USwipeX;
    int m_UInputColorSpaceA;
    int m_UInputColorSpaceB;

    /* Render Texture */
    unsigned int m_TextureA;
    unsigned int m_TextureB;

    /* Internal Formats for GL */
    unsigned int m_InternalFormatA;
    unsigned int m_InternalFormatB;

private: /* Members */
    void InitTextureA(const SharedPixels& image);
    void InitTextureB(const SharedPixels& image);

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

#endif // _VOID_IMAGE_COMPARISON_RENDER_LAYER_H
