// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TEXT_RENDERER_H
#define _VOID_TEXT_RENDERER_H

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

/* Internal */
#include "Definition.h"
#include "VoidRenderer/Core/RenderTypes.h"
#include "VoidRenderer/Programs/TextShaderProgram.h"
#include "VoidRenderer/Programs/TextboxShaderProgram.h"

VOID_NAMESPACE_OPEN

class TextAnnotationsRenderLayer
{

public:
    TextAnnotationsRenderLayer();
    ~TextAnnotationsRenderLayer();

    /* Initializes the shaders and everything */
    void Initialize();

    /* Text Annotation */
    void Begin(const glm::vec2& position);
    /* Add text to the current draft of text */
    void Type(const std::string& text);
    /* Removes A character towards the left */
    void Backspace();
    /* Removes A character towards the right */
    void Delete();
    /* Saves the Current Text into the Annotation */
    void Commit();
    /* Discards Any currently written text */
    void Discard();

    void MoveCaretHome();
    void MoveCaretLeft();
    void MoveCaretRight();
    void MoveCaretEnd();

    /* Draw the Points */
    void Render(const glm::mat4& projection);

    /**
     * Returns if there is an active annotation present
     */
    bool HasAnnotation() { return bool(m_Annotation); }

    /* Clear the strokes */
    inline void Clear() { if (m_Annotation) m_Annotation->Clear(); }

    /**
     * Dereference the current annotation from the Renderer Layer
     * This will result in nothing being drawn on the viewport
     */
    inline void DeleteAnnotation() { m_Annotation = nullptr; }

    /* Sets the Current Annotation */
    inline void SetAnnotation(const Renderer::SharedAnnotation& annotation) { m_Annotation = annotation; }

    /**
     * Attributes Setters
     */
    inline void SetColor(const glm::vec3& color) { m_Color = color; }
    void SetAspect(float aspect);
    /* Return the Annotation Color */
    inline glm::vec3 Color() const { return m_Color; }

    /* Returns whether the annotater is currently typing anything */
    inline bool Typing() const { return m_Typing; }
    inline void SetFontSize(const size_t size)
    { 
        m_Size = size * 20; 
        m_CaretOffset = m_Size * 0.1;
    }

private: /* Members */
    Renderer::SharedAnnotation m_Annotation;
    TextShaderProgram* m_Shader;
    TextBoxShaderProgram* m_BoxShader;
    glm::mat4 m_Projection;

    /* Array Buffers */
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_BoxVAO;
    unsigned int m_BoxVBO;

    /* Uniforms */
    int m_UProjection;
    int m_UColor;
    int m_UText;
    int m_UBoxProjection;
    int m_UBoxColor;
    int m_UBoxThickness;
    int m_UBoxMin;
    int m_UBoxMax;

    FT_Face m_Ft_Face;

    bool m_Typing;
    glm::vec3 m_Color;
    size_t m_Size;

    float m_Scale;
    float m_Scalex;
    float m_CaretScalex;
    int m_CaretIndex;
    float m_Caretxpos;
    float m_CaretOffset;

    std::string m_EditText;

private: /* Members */
    void SetupBuffers();
    bool PreDraw();
    void Draw();
    void PostDraw();

    void DrawText(const Renderer::RenderText& text);
    void DrawCurrent(Renderer::RenderText& text);
    void DrawChar(char& c, float& x, float y);
    void DrawCaret(float x, float y);
    void DrawTextbox();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TEXT_RENDERER_H
