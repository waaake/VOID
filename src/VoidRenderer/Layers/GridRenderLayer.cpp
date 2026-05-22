// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cmath>

/* GLEW */
#include <GL/glew.h>

/* GLM */
#include <glm/gtc/matrix_transform.hpp>     // for ortho/scale
#include <glm/gtc/type_ptr.hpp>             // for glm::value_ptr

/* Internal */
#include "GridRenderLayer.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"
#include "VoidRenderer/Core/Error.h"

VOID_NAMESPACE_OPEN

GridRenderLayer::GridRenderLayer()
    : m_Exposure(0.f)
    , m_Gamma(1.f)
    , m_Gain(1.f)
    , m_ChannelMode(5) /* RGBA */
    , m_UProjection(-1)
    , m_UTexture(-1)
    , m_CellWidth(1.f)
    , m_CellHeight(1.f)
    , m_Rows(1)
    , m_Columns(1)
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
{
}

GridRenderLayer::~GridRenderLayer()
{
    glDeleteTextures(m_Textures.size(), m_Textures.data());
}

void GridRenderLayer::Initialize()
{
    m_Shader.Initialize();

    SetupBuffers();

    m_UProjection = glGetUniformLocation(m_Shader.ProgramId(), "uMVP");
    m_UTexture = glGetUniformLocation(m_Shader.ProgramId(), "uTexture");
    m_UExposure = glGetUniformLocation(m_Shader.ProgramId(), "exposure");
    m_UGamma = glGetUniformLocation(m_Shader.ProgramId(), "gamma");
    m_UGain = glGetUniformLocation(m_Shader.ProgramId(), "gain");
    m_UChannelMode = glGetUniformLocation(m_Shader.ProgramId(), "channelMode");
    m_UInputColorSpace = glGetUniformLocation(m_Shader.ProgramId(), "inputColorSpace");
}

void GridRenderLayer::Reset()
{

}

void GridRenderLayer::SetImages(const std::vector<SharedPixels>& images)
{
    if (m_Textures.size() != images.size())
    {
        glDeleteTextures(m_Textures.size(), m_Textures.data());

        m_Textures.clear();
        m_Textures.shrink_to_fit();

        m_TexData.clear();
        m_TexData.shrink_to_fit();

        m_Textures.resize(images.size());
        m_TexData.resize(images.size());

        glGenTextures(images.size(), m_Textures.data());

        m_Rows = std::ceil(std::sqrt(m_Textures.size()));
        m_Columns = std::ceil((float)m_Textures.size() / m_Rows);

        m_CellWidth = 2.f / m_Columns;
        m_CellHeight = 2.f / m_Rows;
    }

    for (int i = 0; i < images.size(); ++i)
    {
        glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            images[i]->GLInternalFormat(),
            images[i]->Width(),
            images[i]->Height(),
            0,
            images[i]->GLFormat(),
            images[i]->GLType(),
            images[i]->Pixels()
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        m_TexData[i] = std::move(ImageData(
            images[i]->Width(),
            images[i]->Height(),
            static_cast<int>(images[i]->InputColorSpace())
        ));
    }
}

void GridRenderLayer::ReinitShaderProgram()
{
    m_Shader.Reinitialize();

    m_UProjection = glGetUniformLocation(m_Shader.ProgramId(), "uMVP");
    m_UTexture = glGetUniformLocation(m_Shader.ProgramId(), "uTexture");
    m_UExposure = glGetUniformLocation(m_Shader.ProgramId(), "exposure");
    m_UGamma = glGetUniformLocation(m_Shader.ProgramId(), "gamma");
    m_UGain = glGetUniformLocation(m_Shader.ProgramId(), "gain");
    m_UChannelMode = glGetUniformLocation(m_Shader.ProgramId(), "channelMode");
    m_UInputColorSpace = glGetUniformLocation(m_Shader.ProgramId(), "inputColorSpace");
}

void GridRenderLayer::Render(const glm::mat4&, float width, float height)
{
    m_Shader.Bind();

    float viewAspect = width / height;
    float cellWidth = m_CellWidth * viewAspect;

    glBindVertexArray(m_VAO);

    glUniform1f(m_UExposure, m_Exposure);
    glUniform1f(m_UGamma, m_Gamma);
    glUniform1f(m_UGain, m_Gain);
    glUniform1i(m_UChannelMode, m_ChannelMode);

    for (int i = 0; i < static_cast<int>(m_Textures.size()); ++i)
    {
        int row = i / m_Columns;
        int col = i % m_Columns;

        float aspect = (float)m_TexData[i].width / m_TexData[i].height;

        float scalex, scaley;
        if (aspect > (cellWidth / m_CellHeight))
        {
            scalex = cellWidth * 0.5f;
            scaley = scalex / aspect;
        }
        else
        {
            scaley = m_CellHeight * 0.5f;
            scalex = scaley * aspect;
        }

        float offsetx = -viewAspect + (col + 0.5f) * cellWidth;
        float offsety = 1.f - (row + 0.5f) * m_CellHeight;

        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(offsetx, offsety, 0.f));
        model = glm::scale(model, glm::vec3(scalex, scaley, 1.f));

        glm::mat4 view = glm::mat4(1.f);
        glm::mat4 projection = glm::ortho(-viewAspect, viewAspect, -1.f, 1.f);

        glm::mat4 mvp = projection * view * model;

        glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform1i(m_UInputColorSpace, m_TexData[i].colorspace);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
        glUniform1i(m_UTexture, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    m_Shader.Release();
}

void GridRenderLayer::SetupBuffers()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    float vertices[16] = {
        // Pos (0)  // Tex (1)
        -1.f, -1.f,  0.f,  1.f,
         1.f, -1.f,  1.f,  1.f,
         1.f,  1.f,  1.f,  0.f,
        -1.f,  1.f,  0.f,  0.f,
    };

    unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 2 * sizeof(float) represents 3rd element from the vertices above, i.e. start after 8 bytes on to the struct
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

VOID_NAMESPACE_CLOSE
