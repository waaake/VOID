// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_IMAGE_COMPARISON_RENDER_GEAR_H
#define _VOID_IMAGE_COMPARISON_RENDER_GEAR_H

/* Internal */
#include "Definition.h"
#include "RenderGear.h"
#include "VoidRenderer/Programs/ImageComparisonShaderProgram.h"

VOID_NAMESPACE_OPEN

class ImageComparisonRenderGear : public RenderGear
{
public:
    ImageComparisonRenderGear();
    ~ImageComparisonRenderGear();

    /**
     * Initializes the shaders and the internals (Vertex Array Objects | Vertex Buffer Objects | Element/Index Buffer Objects)
     * This method gets called from the renderer after the GL context has been initialized
     */
    virtual void Initialize() override;

    /**
     * Reinitializes the shaders and internals (Vertex Array Objects | Vertex Buffer Objects | Element/Index Buffer Objects)
     */
    virtual void Reinitialize() override;

    /**
     * @brief (Re)Allocates the internal PIXEL buffers with the provided size.
     * 
     * @param size Size of the allocation for the buffer.
     * @param buffer The Pixel buffer to allocate to.
     */
    void ReallocatePixelBuffer(std::size_t size, const PixelBuffer& buffer = PixelBuffer::A) override;

    /**
     * @brief Binds the next index of the Pixel buffer for recieving data.
     * 
     * @param buffer The pixel buffer to bind.
     */
    void RebindPixelBuffer(const PixelBuffer& buffer = PixelBuffer::A) override;

    /**
     * @brief Copy the data from the source to the pixel buffer.
     * 
     * @param data The data to be uploaded on the buffer to the GPU for rendering.
     * @param size Size of the data to be copied.
     */
    void WritePixelData(const void* data, std::size_t size) override;

    /**
     * @brief Unbinds the active pixel buffer
     * 
     */
    void UnbindPixelBuffer() override;

protected:
    /**
     * Setup Array Buffers
     * Initialize the Array Buffers to be used in the program
     */
    virtual void SetupBuffers() override;

    /**
     * Pre-Draw Call
     * Setup anything which is required before drawing anything on the screen
     */
    virtual bool PreDraw() override;

    /**
     * The Main Draw Call
     * This is invoked if the PreDraw is successful
     * The data to be drawn is passed to it
     */
    virtual void Draw(const void* data) override;

    /**
     * The Post Draw Call
     * Anything to be cleaned up after the draw is completed can be done here
     * This method always gets called even if the draw isn't called this will be
     */
    virtual void PostDraw() override;


private: /* Members */
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
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_COMPARISON_RENDER_GEAR_H
