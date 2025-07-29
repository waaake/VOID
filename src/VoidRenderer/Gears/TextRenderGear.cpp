#include <iostream>

/* GLEW */
#include <GL/glew.h>

/* GLM */
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr

/* Internal */
#include "RenderTypes.h"
#include "TextRenderGear.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

TextRenderGear::TextRenderGear()
    : m_VAO(0)
    , m_VBO(0)
    , m_UProjection(-1)
    , m_UColor(-1)
    , m_UText(-1)
    , m_FontEngine(FontEngine::Instance())
{
    /* Setup the Font Face */
    m_Ft_Face = m_FontEngine.GetStandardFace();
}

TextRenderGear::~TextRenderGear()
{
    if (m_Shader)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }
}

void TextRenderGear::Initialize()
{
    m_Shader = new TextShaderProgram;

    /* Initialize the Shaders */
    m_Shader->Initialize();

    /* Initialize the Array Buffers */
    SetupBuffers();

    /* Reinit Textures */
    m_FontEngine.ClearTextures();

    /* Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UColor = glGetUniformLocation(m_Shader->ProgramId(), "uColor");
    m_UText = glGetUniformLocation(m_Shader->ProgramId(), "uText");
}

void TextRenderGear::Reinitialize()
{
    /* Re-Initialize the Shader */
    m_Shader->Reinitialize();

    /* Re-Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UColor = glGetUniformLocation(m_Shader->ProgramId(), "uColor");
    m_UText = glGetUniformLocation(m_Shader->ProgramId(), "uText");
}

void TextRenderGear::SetupBuffers()
{
    /* Gen Arrays */
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    /* Bind */
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    /* Buffer Data -- To be actually filled in when we're rendering dynamically */
    /**
     * 6 Vertices per quad 
     * 4 floats per vertex (position and texture coordinates)
     */
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    /* (location 0) position */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* (Location 1) texCoord */
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool TextRenderGear::PreDraw()
{
    /* Use the Shader Program */
    m_Shader->Bind();

    /* Bind the Vertex Array */
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return true;
}

void TextRenderGear::Draw(const void* data)
{
    /* Cast it back to the Renderable Annotation */
    const Renderer::AnnotationRenderData* d = static_cast<const Renderer::AnnotationRenderData*>(data);

    /* Cannot cast it back */
    if (!d)
        return;

    /* Set the Projection on which this will be rendered */
    glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(d->projection));

    /* For all the Texts in the Annotation -> Render them one by one */
    for (const Renderer::RenderText& text: d->annotation->texts)
    {
        DrawText(text);
    }

    /* Draw the Currently being typed text */
    if (!d->annotation->draft.Empty())
        DrawText(d->annotation->draft);
}

void TextRenderGear::DrawText(const Renderer::RenderText& text)
{
    /* This is to align with the overall NDC of the Projection where the range is [-1, 1] */
    float scale = 0.001;
    float scalex = 0.001/1.77;

    /* Beginning Position */
    float x = text.position.x;
    float y = text.position.y;

    /* Set the Color and the text to be used */
    glUniform3fv(m_UColor, 1, glm::value_ptr(text.color));
    /* Texture sampler bound to unit 0 */
    glUniform1i(m_UText, 0);

    /* Set Font face Pixel size */
    FT_Set_Pixel_Sizes(m_Ft_Face, 0, text.size);

    for (char c: text.text)
    {
        /* Cast the Character into something Freetype can understand */
        FT_ULong code = static_cast<FT_ULong>(c);
        /* Get the Renderable Character glyph struct */
        const Character& ch = m_FontEngine.GetChar(m_Ft_Face, code);

        /* Setup the Necessary data to generate the Buffer data for the draw call */
        float xpos = x + ch.bearing.x * scalex;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scalex;
        float h = ch.size.y * scale;

        /* Buffer Data Vertices */
        float vertices[6][4] = {
            /* First Triangle */
            { xpos,     ypos + h,   0.f, 0.f },
            { xpos,     ypos,       0.f, 1.f },
            { xpos + w, ypos,       1.f, 1.f },

            /* Second Triangle */
            { xpos,     ypos + h,   0.f, 0.f },
            { xpos + w, ypos,       1.f, 1.f },
            { xpos + w, ypos + h,   1.f, 0.f }
        };

        /* Set the Active Texture */
        glActiveTexture(GL_TEXTURE0);
        /* Bind the Texture of the glyph to be rendered */
        glBindTexture(GL_TEXTURE_2D, ch.textureId);

        /* Bind the Vertex Buffer*/
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        /* Setup data */
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0][0]);

        /* Draw */
        glDrawArrays(GL_TRIANGLES, 0, 6);

        /**
         * Update the xpos for the next character to be drawn
         * Freetype stores the advance in 26.6 fixed point format (Due to it's efficiency)
         * which is 64 times the amount of pixels to advance
         * ch.advance >> 6 is bitshifting and producing an equal result of ch.advance / 64 but is faster
         */
        x += (ch.advance >> 6) * scalex;
    }
}

void TextRenderGear::PostDraw()
{
    /* Cleanup */
    glBindVertexArray(0);
    m_Shader->Release();
}

VOID_NAMESPACE_CLOSE
