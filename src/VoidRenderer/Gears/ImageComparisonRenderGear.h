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
