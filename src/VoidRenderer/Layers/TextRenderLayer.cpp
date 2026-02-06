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
    , m_BoxVAO(0)
    , m_BoxVBO(0)
    , m_UProjection(-1)
    , m_UColor(-1)
    , m_UText(-1)
    , m_UBoxProjection(-1)
    , m_UBoxColor(-1)
    , m_UBoxThickness(-1)
    , m_UBoxMin(-1)
    , m_UBoxMax(-1)
    , m_Typing(false)
    , m_Color(1.f, 1.f, 1.f)
    , m_Size(40)
    , m_Scale(0.001)
    , m_Scalex(0.001)
    , m_CaretScalex(0.002)
    , m_CaretIndex(0)
    , m_Caretxpos(0.f)
    , m_CaretOffset(4)
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
    if (m_BoxShader)
    {
        delete m_BoxShader;
        m_BoxShader = nullptr;
    }
}

void TextAnnotationsRenderLayer::Begin(const glm::vec2& position)
{
    /* If the Annotation hasn't been set */
    if (!m_Annotation)
        return;

    /**
     * Check if there is any text annotation that already exists in the same position
     * if found, we edit that annotation else we create a new at the position
     */
    for (int i = 0; i < m_Annotation->texts.size(); ++i)
    {
        Renderer::RenderText& r = m_Annotation->texts[i];
        const glm::vec2& a = r.position;
        const glm::vec2 b = { r.position.x + r.length, r.position.y + (r.size * m_Scale) };

        if ( (position.x - a.x) * (position.x - b.x) <= 0 && (position.y - a.y) * (position.y - b.y) <= 0 )
        {
            m_Annotation->draft = std::move(r);
            m_Annotation->draft.active = true;
            m_Annotation->draft.editing = true;

            m_CaretIndex = m_Annotation->draft.text.size();
            m_Typing = true;
            m_EditText = m_Annotation->draft.text;

            // This does invalidate the iterator, so the last step before returning
            m_Annotation->texts.erase(m_Annotation->texts.begin() + i);
            return;
        }
    }

    /* Set Typing as true for typing into the Annotations */
    m_Typing = true;

    /* Setup the draft */
    m_Annotation->draft.color = m_Color;
    m_Annotation->draft.size = m_Size;
    m_Annotation->draft.position = position;
    m_Annotation->draft.active = true;
}

void TextAnnotationsRenderLayer::Type(const std::string& text)
{
    /* Annotation hasn't been set yet */
    if (!m_Annotation)
        return;

    if (m_CaretIndex < m_Annotation->draft.text.size())
        m_Annotation->draft.text.insert(m_CaretIndex, text);
    else
        m_Annotation->draft.text.append(text);

    m_CaretIndex++;
}

void TextAnnotationsRenderLayer::Backspace()
{
    /* Annotation hasn't been set yet */
    if (!m_Annotation || m_Annotation->draft.text.empty())
        return;

    if (m_CaretIndex < m_Annotation->draft.text.size())
        m_Annotation->draft.text.erase(m_CaretIndex - 1, 1);
    else
        m_Annotation->draft.text.pop_back();

    m_CaretIndex--;
}

void TextAnnotationsRenderLayer::Delete()
{
    /* Annotation hasn't been set yet */
    if (!m_Annotation || m_Annotation->draft.text.empty())
        return;

    if (m_CaretIndex < m_Annotation->draft.text.size())
        m_Annotation->draft.text.erase(m_CaretIndex, 1);
}

void TextAnnotationsRenderLayer::Commit()
{
    /* Annotation hasn't been set yet */
    if (!m_Annotation)
        return;

    /* Nothing to save */
    if (m_Annotation->draft.Empty())
        return;

    /* Add to texts */
    m_Annotation->draft.active = false;
    m_Annotation->texts.push_back(std::move(m_Annotation->draft));

    /* Clear draft */
    m_Annotation->draft.Clear();

    /* Not typing anymore */
    m_Typing = false;
    m_CaretIndex = 0;
}

void TextAnnotationsRenderLayer::Discard()
{
    /* Annotation hasn't been set yet */
    if (!m_Annotation)
        return;

    /* Revert to the last text when editing */
    if (m_Annotation->draft.editing)
    {
        m_Annotation->draft.editing = false;
        m_Annotation->draft.text = m_EditText;
        return Commit();
    }

    /* Clear draft */
    m_Annotation->draft.active = false;
    m_Annotation->draft.Clear();

    /* Not typing anymore */
    m_Typing = false;
}

void TextAnnotationsRenderLayer::MoveCaretHome()
{
    m_CaretIndex = 0;
}

void TextAnnotationsRenderLayer::MoveCaretLeft()
{
    m_CaretIndex = std::max(m_CaretIndex - 1, 0);
}

void TextAnnotationsRenderLayer::MoveCaretRight()
{
    m_CaretIndex = std::min(m_CaretIndex + 1, static_cast<int>(m_Annotation->draft.text.size()));
}

void TextAnnotationsRenderLayer::MoveCaretEnd()
{
    m_CaretIndex = static_cast<int>(m_Annotation->draft.text.size());
}

void TextAnnotationsRenderLayer::SetAspect(float aspect)
{
    m_Scalex = m_Scale / aspect;
    m_CaretScalex = 0.0012 / aspect;
}

void TextAnnotationsRenderLayer::Initialize()
{
    m_Shader = new TextShaderProgram;
    m_BoxShader = new TextBoxShaderProgram;

    /* Initialize the Shaders */
    m_Shader->Initialize();
    m_BoxShader->Initialize();

    /* Initialize the Array Buffers */
    SetupBuffers();

    /* Reinit Textures */
    FontEngine::Instance().ClearTextures();

    /* Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UColor = glGetUniformLocation(m_Shader->ProgramId(), "uColor");
    m_UText = glGetUniformLocation(m_Shader->ProgramId(), "uText");

    m_UBoxProjection = glGetUniformLocation(m_BoxShader->ProgramId(), "uMVP");
    m_UBoxColor = glGetUniformLocation(m_BoxShader->ProgramId(), "uColor");
    m_UBoxThickness = glGetUniformLocation(m_BoxShader->ProgramId(), "uThickness");
    m_UBoxMin = glGetUniformLocation(m_BoxShader->ProgramId(), "uMin");
    m_UBoxMax = glGetUniformLocation(m_BoxShader->ProgramId(), "uMax");
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

    /* Textbox */
    glGenVertexArrays(1, &m_BoxVAO);
    glGenBuffers(1, &m_BoxVBO);

    glBindVertexArray(m_BoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_BoxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

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
    glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(m_Projection));

    for (const Renderer::RenderText& text: m_Annotation->texts)
        DrawText(text);

    /* Draw the Currently being typed text */
    if (m_Annotation->draft.Active())
        DrawCurrent(m_Annotation->draft);
}

void TextAnnotationsRenderLayer::DrawText(const Renderer::RenderText& text)
{
    /* Beginning Position */
    float x = text.position.x;
    float y = text.position.y;

    /* Set the Color and the text to be used */
    glUniform3fv(m_UColor, 1, glm::value_ptr(text.color));
    glUniform1i(m_UText, 0);

    FT_Set_Pixel_Sizes(m_Ft_Face, 0, text.size);

    for (char c : text.text)
        DrawChar(c, x, y);
}

void TextAnnotationsRenderLayer::DrawCurrent(Renderer::RenderText& text)
{
    /* Beginning Position */
    float x = text.position.x;
    float y = text.position.y;

    m_Caretxpos = x - (m_CaretOffset * m_Scalex);

    /* Set the Color and the text to be used */
    glUniform3fv(m_UColor, 1, glm::value_ptr(text.color));
    glUniform1i(m_UText, 0);

    FT_Set_Pixel_Sizes(m_Ft_Face, 0, text.size);

    int count = 0;

    for (char c : text.text)
    {
        DrawChar(c, x, y);

        if (count < m_CaretIndex)
        {
            m_Caretxpos = x - (m_CaretOffset * m_Scalex);
            count++;
        }
    }

    text.length = std::abs(x - text.position.x);

    DrawCaret(m_Caretxpos, y);
    DrawTextbox();
}

void TextAnnotationsRenderLayer::PostDraw()
{
    /* Cleanup */
    glBindVertexArray(0);
    m_Shader->Release();
}

void TextAnnotationsRenderLayer::DrawChar(char& c, float& x, float y)
{
    const Character& ch = FontEngine::Instance().GetChar(m_Ft_Face, static_cast<FT_ULong>(c));

    float xpos = x + ch.bearing.x * m_Scalex;
    float ypos = y - (ch.size.y - ch.bearing.y) * m_Scale;

    float w = ch.size.x * m_Scalex;
    float h = ch.size.y * m_Scale;

    /* Buffer Data */
    float vertices[6][4] = {
        /* First */
        { xpos,     ypos + h,   0.f, 0.f },
        { xpos,     ypos,       0.f, 1.f },
        { xpos + w, ypos,       1.f, 1.f },

        /* Second */
        { xpos,     ypos + h,   0.f, 0.f },
        { xpos + w, ypos,       1.f, 1.f },
        { xpos + w, ypos + h,   1.f, 0.f }
    };

    glActiveTexture(GL_TEXTURE0);
    /* Bind the Texture of the glyph to be rendered */
    glBindTexture(GL_TEXTURE_2D, ch.textureId);

    /* Bind the Vertex Buffer & Setup data */
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /**
     * Update the xpos for the next character to be drawn
     * Freetype stores the advance in 26.6 fixed point format (Due to it's efficiency)
     * which is 64 times the amount of pixels to advance
     */
    x += (ch.advance >> 6) * m_Scalex;
}

void TextAnnotationsRenderLayer::DrawCaret(float x, float y)
{
    const Character& ch = FontEngine::Instance().GetChar(m_Ft_Face, static_cast<FT_ULong>('|'));

    /* Setup the Necessary data to generate the Buffer data for the draw call */
    float xpos = x + ch.bearing.x * m_Scalex;
    float ypos = y - (ch.size.y - ch.bearing.y) * m_Scale;

    float w = ch.size.x * m_Scalex;
    float h = ch.size.y * m_Scale;

    /* Buffer Data */
    float vertices[6][4] = {
        /* First */
        { xpos,     ypos + h,   0.f, 0.f },
        { xpos,     ypos,       0.f, 1.f },
        { xpos + w, ypos,       1.f, 1.f },

        /* Second */
        { xpos,     ypos + h,   0.f, 0.f },
        { xpos + w, ypos,       1.f, 1.f },
        { xpos + w, ypos + h,   1.f, 0.f }
    };

    glActiveTexture(GL_TEXTURE0);
    /* Bind the Texture of the glyph to be rendered */
    glBindTexture(GL_TEXTURE_2D, ch.textureId);

    /* Bind the Vertex Buffer & Setup data */
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void TextAnnotationsRenderLayer::DrawTextbox()
{
    m_BoxShader->Bind();

    const Renderer::RenderText& r = m_Annotation->draft;

    const float halfsize = r.size * 0.5 * m_Scale;
    const glm::vec2 a = { r.position.x - halfsize, r.position.y - halfsize };
    const glm::vec2 b = { r.position.x + r.length + halfsize , r.position.y + (r.size * m_Scale) };

    /**
     * a ________
     *  |        |
     *  |________|
     *            b
     */
    float vertices[8] = {
        a.x, a.y,
        b.x, a.y,
        b.x, b.y,
        a.x, b.y
    };

    glUniformMatrix4fv(m_UBoxProjection, 1, GL_FALSE, glm::value_ptr(m_Projection));
    glUniform2f(m_UBoxMin, a.x, a.y);
    glUniform2f(m_UBoxMax, b.x, b.y);
    glUniform1f(m_UBoxThickness, 0.003f);
    glUniform3f(m_UBoxColor, 1.f, 1.f, 1.f);

    glBindVertexArray(m_BoxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_BoxVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    m_BoxShader->Release();
}

VOID_NAMESPACE_CLOSE
