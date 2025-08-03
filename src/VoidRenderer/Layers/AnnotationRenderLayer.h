// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_STROKE_RENDERER_H
#define _VOID_STROKE_RENDERER_H

/* Glew */
#include <GL/glew.h>

/* STD */
#include <vector>

/* Freetype */
#include <ft2build.h>
#include FT_FREETYPE_H

/* GLM */
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

/* Qt */
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidRenderer/RenderTypes.h"
#include "VoidRenderer/Core/FontEngine.h"
#include "VoidRenderer/Gears/StrokeRenderGear.h"
#include "VoidRenderer/Gears/TextRenderGear.h"

VOID_NAMESPACE_OPEN

class VoidAnnotationsRenderer
{

public:
    VoidAnnotationsRenderer();
    ~VoidAnnotationsRenderer();

    /* Initializes the shaders and everything */
    void Initialize();

    /* Add a point to the annotation currently being drawn */
    void DrawPoint(const glm::vec2& point);

    /* Commit the last drawn annotation into a stroke */
    void CommitStroke();

    /* Remove a stroke which collides with the point */
    void EraseStroke(const glm::vec2& point);

    /* Text Annotation */
    void BeginTyping(const glm::vec2& position);
    /* Add text to the current draft of text */
    void Type(const std::string& text);
    /* Removes A character */
    void Backspace();
    /* Saves the Current Text into the Annotation */
    void CommitText();
    /* Discards Any currently written text */
    void DiscardText();

    /* Draw the Points */
    void Render(const glm::mat4& projection);

    /**
     * Returns if there is an active annotation present
     */
    bool HasAnnotation() { return bool(m_Annotation); }
    /**
     * Creates a New Annotation and sets it for the Drawing
     * Returns the SharedPointer back
     */
    Renderer::SharedAnnotation NewAnnotation();

    /* Clear the strokes */
    inline void Clear()
    { 
        if (m_Annotation)
            m_Annotation->Clear();
    }

    /**
     * Dereference the current annotation from the Renderer Layer
     * This will result in nothing being drawn on the viewport
     */
    inline void DeleteAnnotation() { m_Annotation = nullptr; }

    /* Sets the Current Annotation */
    inline void SetAnnotation(const Renderer::SharedAnnotation& annotation)
    {
        m_Annotation = annotation; 
        m_AnnotationData->annotation = annotation;
    }
    
    /**
     * Attributes Setters
     */
    inline void SetColor(const glm::vec3& color) { m_Color = color; }
    inline void SetColor(const QColor& color)
    {
        /* This Renderer takes colors normalized to 0.f - 1.f */
        m_Color = { color.red() / 255.f, color.green() / 255.f, color.blue() / 255.f };
    }
    /* Return the Annotation Color */
    inline glm::vec3 Color() const { return m_Color; }

    inline void SetBrushSize(const float size) { m_Size = size / 500; }
    inline float BrushSize() const { return m_Size; }

    /* The Annotation Draw Type */
    inline void SetDrawType(const Renderer::DrawType& type) { m_DrawType = type; }
    inline const Renderer::DrawType& DrawType() const { return m_DrawType; }

    /* Returns whether the annotater is currently typing anything */
    inline bool Typing() const { return m_Typing; }

    inline void SetFontSize(const size_t size) { m_FontSize = size * 20; }

private: /* Members */
    /* Current Annotation to be renderer / Updated during a draw */
    Renderer::SharedAnnotation m_Annotation;

    /* Last Point -- used to calculate the direction/normal */
    glm::vec2 m_LastPoint;

    /* Indicates whether the draw has been started for the annotation */
    bool m_Drawing;

    /* Indicates that we're typing text for the annoation */
    bool m_Typing;

    /* Annotation Attributes */
    glm::vec3 m_Color;
    float m_Size;

    /* Renderable Annotation Data */
    Renderer::AnnotationRenderData* m_AnnotationData;

    /* The Current Draw Type */
    Renderer::DrawType m_DrawType;

    /* Render Components */
    StrokeRenderGear* m_StrokeRenderer;
    TextRenderGear* m_TextRenderer;

    /* Font */
    size_t m_FontSize;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_STROKE_RENDERER_H
