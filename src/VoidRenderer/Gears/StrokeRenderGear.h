#ifndef _VOID_STROKE_RENDER_GEAR_H
#define _VOID_STROKE_RENDER_GEAR_H

/* GLM */
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr

/* Internal */
#include "Definition.h"
#include "RenderGear.h"
#include "VoidRenderer/Programs/StrokeShaderProgram.h"

VOID_NAMESPACE_OPEN

class StrokeRenderGear : public RenderGear
{
public:
    StrokeRenderGear();
    ~StrokeRenderGear();

    /**
     * Initializes the shaders and the internals (Vertex Array Objects | Vertex Buffer Objects | Element/Index Buffer Objects)
     * This method gets called from the renderer after the GL context has been initialized
     */
    virtual void Initialize() override;

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
    StrokeShaderProgram* m_Shader;

    /* Array Buffers */
    unsigned int m_VAO;
    unsigned int m_VBO;

    /* Uniforms */
    int m_UProjection;
    int m_UColor;
    int m_USize;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_STROKE_RENDER_GEAR_H
