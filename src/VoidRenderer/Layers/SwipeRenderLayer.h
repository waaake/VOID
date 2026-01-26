// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SWIPE_RENDER_LAYER_H
#define _SWIPE_RENDER_LAYER_H

/* Internal */
#include "Definition.h"
#include "VoidRenderer/RenderTypes.h"
#include "VoidRenderer/Programs/SwiperShaderProgram.h"

VOID_NAMESPACE_OPEN

class SwipeRenderLayer
{
public:
    SwipeRenderLayer();
    ~SwipeRenderLayer();

    /* Sets up the Render Components (Gears) */
    void Initialize();

    void Reset();

    inline float SwipeX() const { return m_SwipeX; }
    inline void SetSwipeX(const float x) { m_SwipeX = x; }

    inline float SwipeOffset() const { return m_Offset; }
    inline void SetSwipeOffset(const float x) { m_Offset = x; }

    void ReinitShaderProgram();

    /* Main Render Function */
    void Render();

private: /* Members */
    float m_SwipeX;
    float m_Offset;

    /* Render Components */
    SwiperShaderProgram* m_Shader;

    /**
     * Array and Buffer objects
     *
     * Vertex array Object
     * Vertex Buffer Object
     * Element or the index buffer object
     */
    unsigned int m_VAO;
    unsigned int m_VBO;

    /* Uniforms */
    int m_UProjection;
    int m_UColor;

private: /* Members */
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

#endif // _SWIPE_RENDER_LAYER_H
