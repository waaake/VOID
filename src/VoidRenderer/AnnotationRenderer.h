#ifndef _VOID_STROKE_RENDERER_H
#define _VOID_STROKE_RENDERER_H

/* Glew */
#include <GL/glew.h>

/* STD */
#include <vector>

/* GLM */
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

/* Qt */
#include <QColor>
#include <QOpenGLShaderProgram>

/* Internal */
#include "Definition.h"
#include "RenderTypes.h"
#include "Programs/StrokeShaderProgram.h"
#include "Gears/StrokeRenderGear.h"

VOID_NAMESPACE_OPEN

class VoidAnnotationsRenderer
{

public: /* Enums */
    enum class DrawType
    {
        /* Nothing */
        NONE,
        /* Draw/Annotate using a Brush */
        BRUSH,
        /* Draw Text */
        TEXT,
        /* Erase Strokes/Text */
        ERASER
    };

public:
    VoidAnnotationsRenderer();
    ~VoidAnnotationsRenderer();

    /**
     * Initializes the shaders and everything
     */
    void Initialize();

    /**
     * Add a point to the annotation currently being drawn
     */
    bool DrawPoint(const glm::vec2& point);

    /**
     * Commit the last drawn annotation into a stroke
     */
    void CommitStroke();

    /**
     * Draw the Points
     */
    void Render(const glm::mat4& projection);

    /**
     * Creates a New Annotation and sets it for the Drawing
     * Returns the SharedPointer back
     */
    Renderer::SharedAnnotation NewAnnotation();

    /**
     * Clear the strokes
     */
    inline void Clear()
    { 
        if (m_Annotation)
            m_Annotation->Clear();
    }

    /* Sets the Current Annotation */
    inline void SetAnnotation(const Renderer::SharedAnnotation& annotation) { m_Annotation = annotation; }
    
    /**
     * Attributes Setters
     */
    inline void SetColor(const glm::vec3& color) { m_Color = color; }
    inline void SetColor(const QColor& color)
    {
        /* This Renderer takes colors normalized to 0.f - 1.f */
        m_Color = { color.red() / 255.f, color.green() / 255.f, color.blue() / 255.f };
    }

    inline void SetAnnotationBrushSize(const float size) { m_Size = size; }

private: /* Members */
    // std::vector<Renderer::Stroke> m_Strokes;

    // /**
    //  * Stores the Active Annotation
    //  */
    // std::vector<Renderer::AnnotatedVertex> m_Annotation;
    Renderer::SharedAnnotation m_Annotation;

    /**
     * Last Point -- used to calculate the direction/normal
     */
    glm::vec2 m_LastPoint;

    /**
     * Indicates whether the draw has been started for the annotation
     */
    bool m_Drawing;

    /**
     * Annotation Attributes
     */
    glm::vec3 m_Color;
    float m_Size;

    /* The Current Draw Type */
    DrawType m_DrawType;

    /* Shaders */
    StrokeShaderProgram* m_StrokeShader;
    // QOpenGLShaderProgram* m_Shader;

    unsigned int m_VAO;
    unsigned int m_VBO;

    /* Uniform Locations for uniform variables within the shaders */
    int m_MVPLoc;
    int m_ThicknessLoc;
    int m_ColorLoc;

private: /* Methods */
    /* Load the Shaders for Strokes */
    // bool LoadShaders();
    /* Setup the Vertex Buffers and Arrays */
    void SetupArrays();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_STROKE_RENDERER_H
