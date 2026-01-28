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

VOID_NAMESPACE_OPEN

class TextAnnotationsRenderLayer
{

public:
    TextAnnotationsRenderLayer();
    ~TextAnnotationsRenderLayer();

    /* Initializes the shaders and everything */
    void Initialize();

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
    /* Return the Annotation Color */
    inline glm::vec3 Color() const { return m_Color; }

    /* Returns whether the annotater is currently typing anything */
    inline bool Typing() const { return m_Typing; }
    inline void SetFontSize(const size_t size) { m_Size = size * 20; }

private: /* Members */
    Renderer::SharedAnnotation m_Annotation;
    TextShaderProgram* m_Shader;
    glm::mat4 m_Projection;

    /* Array Buffers */
    unsigned int m_VAO;
    unsigned int m_VBO;

    /* Uniforms */
    int m_UProjection;
    int m_UColor;
    int m_UText;

    FT_Face m_Ft_Face;

    bool m_Typing;
    glm::vec3 m_Color;
    size_t m_Size;

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
    
     /**
      * @brief Renders a chunk of text (typically a word or statement).
      * 
      * @param text The RenderText info.
      */
    void DrawText(const Renderer::RenderText& text);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TEXT_RENDERER_H
