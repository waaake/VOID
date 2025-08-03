// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* GLM */
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr


/* Internal */
#include "RenderTypes.h"
#include "SwiperRenderGear.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SwiperRenderGear::SwiperRenderGear()
    : m_VAO(0)
    , m_VBO(0)
    , m_UProjection(-1)
    , m_UColor(-1)
{
}

SwiperRenderGear::~SwiperRenderGear()
{
    if (m_Shader)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }
}

void SwiperRenderGear::Initialize()
{
    m_Shader = new SwiperShaderProgram;

    /* Initialize the Shaders */
    m_Shader->Initialize();

    /* Initialize the Array Buffers */
    SetupBuffers();

    /* Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UColor = glGetUniformLocation(m_Shader->ProgramId(), "uColor");
}

void SwiperRenderGear::Reinitialize()
{
    /* Re-Initialize the Shader */
    m_Shader->Reinitialize();

    /* Re-Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UColor = glGetUniformLocation(m_Shader->ProgramId(), "uColor");
}

void SwiperRenderGear::SetupBuffers()
{
    float vertices[6] = {
        0.f, -1.f, 0.f,
        0.f,  1.f, 0.f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    /* Bind */
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    /* Buffer data */
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool SwiperRenderGear::PreDraw()
{
    /* Use the Shader Program */
    m_Shader->Bind();

    /* Bind the Vertex Array */
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);

    return true;
}

void SwiperRenderGear::Draw(const void* data)
{
    /* Cast it back to the Image data for comparison */
    const Renderer::ImageComparisonRenderData* d = static_cast<const Renderer::ImageComparisonRenderData*>(data);

    /* Can't cast */
    if (!d)
        return;

    /**
     * Normalize between the device coordinates
     * the SwipeX limit is between 0.0 - 1.0
     * to make it work in OpenGL's Normalized coordinates -1.0 - 1.0
     * the math needs to be applied
     *
     * lowest: (0.f * 2.f) - 1.f = -1.f;
     * mid: (0.5f * 2.f) - 1.f = 0.f;
     * highest: (1.f * 2.f) - 1.f = 1.f;
     */
    float normalized = ((d->swipeX + d->offset) * 2.f) - 1.f;

    /* Model view projection for the swiper */
    glm::mat4 projection = glm::ortho(-1.f, 1.f, -1.f, 1.f);
    glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(normalized, 0.f, 0.f));
    glm::mat4 mvp = projection * model;

    /* Set Model View Projection Matrix */
    glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(mvp));
    /* Set the Color */
    float color[3] = {1.f, 1.f, 1.f};
    glUniform3fv(m_UColor, 1, color);

    /* Draw */
    glDrawArrays(GL_LINES, 0, 2);
}

void SwiperRenderGear::PostDraw()
{
    /* Cleanup */
    glBindVertexArray(0);
    m_Shader->Release();
}

VOID_NAMESPACE_CLOSE
