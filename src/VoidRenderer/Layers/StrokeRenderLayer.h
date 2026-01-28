// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_ANNOTATION_STROKE_RENDERER_H
#define _VOID_ANNOTATION_STROKE_RENDERER_H

/* Glew */
#include <GL/glew.h>

/* STD */
#include <vector>

/* GLM */
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

/* Internal */
#include "Definition.h"
#include "VoidRenderer/Core/RenderTypes.h"
#include "VoidRenderer/Programs/StrokeShaderProgram.h"

VOID_NAMESPACE_OPEN

class StrokeRenderLayer
{

public:
    StrokeRenderLayer();
    ~StrokeRenderLayer();

    /* Initializes the shaders and everything */
    void Initialize();

    /* Add a point to the annotation currently being drawn */
    void DrawPoint(const glm::vec2& point);

    /* Commit the last drawn annotation into a stroke */
    void CommitStroke();

    /* Remove a stroke which collides with the point */
    void EraseStroke(const glm::vec2& point);

    /* Draw the Points */
    void Render(const glm::mat4& projection);

    /**
     * Returns if there is an active annotation present
     */
    bool HasAnnotation() { return bool(m_Annotation); }

    /**
     * Dereference the current annotation from the Renderer Layer
     * This will result in nothing being drawn on the viewport
     */
    inline void DeleteAnnotation() { m_Annotation = nullptr; }
    inline void SetAnnotation(const Renderer::SharedAnnotation& annotation) { m_Annotation = annotation; }
    
    /**
     * Attributes Setters
     */
    inline void SetColor(const glm::vec3& color) { m_Color = color; }
    /* Return the Annotation Color */
    inline glm::vec3 Color() const { return m_Color; }

    inline void SetBrushSize(const float size) { m_Size = size / 500; }
    inline float BrushSize() const { return m_Size; }

private: /* Members */
    Renderer::SharedAnnotation m_Annotation;
    StrokeShaderProgram* m_Shader;

    glm::mat4 m_Projection;

    /* Array Buffers */
    unsigned int m_VAO;
    unsigned int m_VBO;

    /* Uniforms */
    int m_UProjection;
    int m_UColor;
    int m_USize;

    glm::vec2 m_LastPoint;
    glm::vec3 m_Color;

    bool m_Drawing;
    float m_Size;

private: /* Methods */
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

    /**
     * @brief Draws the current stroke on the gl buffer
     * 
     * @param stroke The Stroke to be rendererd.
     */
    void DrawStroke(const Renderer::Stroke& stroke);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_ANNOTATION_STROKE_RENDERER_H
