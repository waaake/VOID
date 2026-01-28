// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QFile>

/* GLM */
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr

/* Internal */
#include "TextRenderLayer.h"
#include "VoidCore/Logging.h"
#include "VoidRenderer/Core/FontEngine.h"

VOID_NAMESPACE_OPEN

TextAnnotationsRenderLayer::TextAnnotationsRenderLayer()
    : m_Annotation(nullptr)
    , m_Shader(nullptr)
    , m_VAO(0)
    , m_VBO(0)
    , m_UProjection(-1)
    , m_UColor(-1)
    , m_UText(-1)
    , m_Typing(false)
    , m_Color(1.f, 1.f, 1.f)
    , m_Size(40)
{
    m_Ft_Face = FontEngine::Instance().GetStandardFace();
}

TextAnnotationsRenderLayer::~TextAnnotationsRenderLayer()
{
    if (m_Shader)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }
}

void TextAnnotationsRenderLayer::BeginTyping(const glm::vec2& position)
{
    /* If the Annotation hasn't been set */
    if (!m_Annotation)
        return;
    
    /* Set Typing as true for typing into the Annotations */
    m_Typing = true;

    /* Setup the draft */
    m_Annotation->draft.color = m_Color;
    m_Annotation->draft.size = m_Size;
    m_Annotation->draft.position = position;
}

void TextAnnotationsRenderLayer::Type(const std::string& text)
{
    /* Annotation hasn't been set yet */
    if (!m_Annotation)
        return;

    m_Annotation->draft.text.append(text);
}

void TextAnnotationsRenderLayer::Backspace()
{
    /* Annotation hasn't been set yet */
    if (!m_Annotation)
        return;

    m_Annotation->draft.text.pop_back();
}

void TextAnnotationsRenderLayer::CommitText()
{
    /* Annotation hasn't been set yet */
    if (!m_Annotation)
        return;

    /* Nothing to save */
    if (m_Annotation->draft.Empty())
        return;

    /* Add to texts */
    m_Annotation->texts.push_back(std::move(m_Annotation->draft));

    /* Clear draft */
    m_Annotation->draft.Clear();

    /* Not typing anymore */
    m_Typing = false;
}

void TextAnnotationsRenderLayer::DiscardText()
{
    /* Annotation hasn't been set yet */
    if (!m_Annotation)
        return;

    /* Clear draft */
    m_Annotation->draft.Clear();

    /* Not typing anymore */
    m_Typing = false;
}

void TextAnnotationsRenderLayer::Initialize()
{
    m_Shader = new TextShaderProgram;

    /* Initialize the Shaders */
    m_Shader->Initialize();

    /* Initialize the Array Buffers */
    SetupBuffers();

    /* Reinit Textures */
    FontEngine::Instance().ClearTextures();

    /* Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UColor = glGetUniformLocation(m_Shader->ProgramId(), "uColor");
    m_UText = glGetUniformLocation(m_Shader->ProgramId(), "uText"); 
}

void TextAnnotationsRenderLayer::Render(const glm::mat4& projection)
{
    m_Projection = projection;

    if (PreDraw())
        Draw();
    
    PostDraw();
}

void TextAnnotationsRenderLayer::SetupBuffers()
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

bool TextAnnotationsRenderLayer::PreDraw()
{
    /* Use the Shader Program */
    m_Shader->Bind();

    /* Bind the Vertex Array */
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return true;
}

void TextAnnotationsRenderLayer::Draw()
{
    /* Set the Projection on which this will be rendered */
    glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(m_Projection));

    /* For all the Texts in the Annotation -> Render them one by one */
    for (const Renderer::RenderText& text: m_Annotation->texts)
        DrawText(text);

    /* Draw the Currently being typed text */
    if (!m_Annotation->draft.Empty())
        DrawText(m_Annotation->draft);
}

void TextAnnotationsRenderLayer::DrawText(const Renderer::RenderText& text)
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
        const Character& ch = FontEngine::Instance().GetChar(m_Ft_Face, code);

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

void TextAnnotationsRenderLayer::PostDraw()
{
    /* Cleanup */
    glBindVertexArray(0);
    m_Shader->Release();
}

VOID_NAMESPACE_CLOSE
