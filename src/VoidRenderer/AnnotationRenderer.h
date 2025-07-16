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
#include "RenderTypes.h"
#include "VoidRenderer/Gears/StrokeRenderGear.h"
#include "VoidRenderer/Gears/TextRenderGear.h"

VOID_NAMESPACE_OPEN

/**
 * Font Reader for loading fonts via QResource
 * Reads all of the Font file data
 * Then provides the data to Freetype FT_New_Memory_Face
 * internally keeps the data alive till its needed
 */
class FontReader
{
public:
    FontReader() = default;

    bool Read(const std::string& path);
    QByteArray& Data() { return m_Data; }

private:
    QByteArray m_Data;

};

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

    /* Sets the Current Font Face */
    void SetFontFace(const std::string& path, const int size);

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

    inline void SetFontSize(const int size)
    {
        /* Update the Font size */
        m_FontSize = size * 20;
        /* Reload the Font */
        SetFontFace(m_FontPath, m_FontSize);
    }

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

    /* Text Attributes */
    FT_Library m_FtLib;
    FT_Face m_FontFace;

    /* Renderable Annotation Data */
    Renderer::AnnotationRenderData* m_AnnotationData;

    /* The Current Draw Type */
    Renderer::DrawType m_DrawType;

    /* Render Components */
    StrokeRenderGear* m_StrokeRenderer;
    TextRenderGear* m_TextRenderer;

    /* Fonts */
    std::string m_FontPath;
    int m_FontSize;
    FontReader m_FontReader;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_STROKE_RENDERER_H
