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
    : m_UProjection(-1)
    , m_UTexture(-1)
    , m_CellWidth(1.f)
    , m_CellHeight(1.f)
    , m_Rows(1)
    , m_Columns(1)
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
}

void GridRenderLayer::Reset()
{

}

void GridRenderLayer::SetImages(const std::vector<SharedPixels>& images)
{
    // VOID_LOG_INFO("GridRenderLayer::SetImages");
    Tools::VoidProfiler<std::chrono::duration<double>> p("GridRenderLayer::SetImages");
    if (m_Textures.size() != images.size())
    {
        glDeleteTextures(m_Textures.size(), m_Textures.data());

        m_Textures.clear();
        m_Textures.shrink_to_fit();

        m_TexSizes.clear();
        m_TexSizes.shrink_to_fit();

        m_Textures.resize(images.size());
        m_TexSizes.reserve(images.size());

        glGenTextures(images.size(), m_Textures.data());

        m_Rows = std::ceil(std::sqrt(m_Textures.size()));
        m_Columns = std::ceil((float)m_Textures.size() / m_Rows);

        m_CellWidth = 2.f / m_Columns;
        m_CellHeight = 2.f / m_Rows;

        // SetupBuffers();
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

        m_TexSizes.emplace_back(images[i]->Width(), images[i]->Height());
    }
}

void GridRenderLayer::ReinitShaderProgram()
{
    m_Shader.Reinitialize();

    m_UProjection = glGetUniformLocation(m_Shader.ProgramId(), "uMVP");
    m_UTexture = glGetUniformLocation(m_Shader.ProgramId(), "uTexture");
}

void GridRenderLayer::Render(const glm::mat4&, float width, float height)
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("GridRenderLayer::Render");
    // m_Projection = projection;
    m_Shader.Bind();

    // // int max = static_cast<int>(m_Textures.size());

    // // int rows = std::ceil(std::sqrt(max));
    // // int cols = std::ceil((float)max / rows);

    // // float cwidth = 2.f / cols;
    // // float cheight = 2.f / rows;

    // for (int i = 0; i < static_cast<int>(m_Textures.size()); ++i)
    // {
    //     // int row = i / m_Columns;
    //     // int col = i % m_Columns;

    //     // float x0 = -1.f + col * m_CellWidth;
    //     // float y0 = 1.f - row * m_CellHeight;
    //     // float x1 = x0 + m_CellWidth;
    //     // float y1 = y0 - m_CellHeight;

    //     // float vertices[16] = {
    //     //     x0, y0, 0.f, 0.f,
    //     //     x1, y0, 1.f, 0.f,
    //     //     x1, y1, 1.f, 1.f,
    //     //     x0, y1, 0.f, 1.f,
    //     // };

    //     // unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

    //     // unsigned int vao, vbo, ebo;
    //     // glGenVertexArrays(1, &vao);
    //     // glGenBuffers(1, &vbo);
    //     // glGenBuffers(1, &ebo);

    //     // glBindVertexArray(vao);

    //     // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //     // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //     // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //     // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //     // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    //     // glEnableVertexAttribArray(0);

    //     // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    //     // glEnableVertexAttribArray(1);

    //     // glActiveTexture(GL_TEXTURE0);

    //     // glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
    //     // glUniform1i(m_UTexture, 0);

    //     // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //     // glBindTexture(GL_TEXTURE_2D, 0);
    //     // glDeleteVertexArrays(1, &vao);
    //     // glDeleteBuffers(1, &vbo);
    //     // glDeleteBuffers(1, &ebo);

    //     // Pre draw
    //     glBindVertexArray(m_Vaos[i]);
    //     glEnableVertexAttribArray(0);
    //     glEnableVertexAttribArray(1);

    //     // Draw
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
    //     glUniform1i(m_UTexture, 0);
    //     Renderer::GLGetError("GridRenderLayer");

    //     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //     // Post Draw
    //     // glBindVertexArray(0);
    //     // glBindTexture(GL_TEXTURE_2D, 0);
    // }

    // float cellWidth = width / m_Columns;
    // float cellHeight = height / m_Rows;
    // m_CellWidth = width / m_Columns;
    // m_CellHeight = height / m_Rows;
    float viewAspect = width / height;
    float cellWidth = m_CellWidth * viewAspect;
    // float cellWidth = (2.f * viewAspect) / m_Columns;
    // float cellHeight = 2.f / m_Rows;

    glBindVertexArray(m_VAO);

    for (int i = 0; i < static_cast<int>(m_Textures.size()); ++i)
    {
        int row = i / m_Columns;
        int col = i % m_Columns;

        float aspect = (float)m_TexSizes[i].first / m_TexSizes[i].second;
        // float cellWidth = width / m_Columns;
        // float cellHeight = height / m_Rows;

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

        // VOID_LOG_INFO("W: {0}, H: {1}, A: {2}, Sx: {3}, Sy: {4}", m_TexSizes[i].first, m_TexSizes[i].second, aspect, scalex, scaley);

        float offsetx = -viewAspect + (col + 0.5f) * cellWidth;
        float offsety = 1.f - (row + 0.5f) * m_CellHeight;

        // float offsetx = -1.f + (col + 0.5f) * (2.f / m_Columns);
        // float offsety = 1.f - (row + 0.5f) * (2.f / m_Rows);

        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(offsetx, offsety, 0.f));
        model = glm::scale(model, glm::vec3(scalex, scaley, 1.f));

        glm::mat4 view = glm::mat4(1.f);
        glm::mat4 projection = glm::ortho(-viewAspect, viewAspect, -1.f, 1.f);

        glm::mat4 mvp = projection * view * model;

        // glm::mat4 mvp = ModelViewProjectionMat(m_TexSizes[i].first, m_TexSizes[i].second, offsetx, offsety);

        glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(mvp));

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
    // Delete existing
    // if (m_Vaos.size() > m_Textures.size())
    // {
    //     glDeleteVertexArrays(m_Vaos.size(), m_Vaos.data());
    //     glDeleteBuffers(m_Vbos.size(), m_Vbos.data());
    //     glDeleteBuffers(m_Ebos.size(), m_Ebos.data());
    // }

    // Delete existing
    // glDeleteVertexArrays(m_Vaos.size(), m_Vaos.data());
    // glDeleteBuffers(m_Vbos.size(), m_Vbos.data());
    // glDeleteBuffers(m_Ebos.size(), m_Ebos.data());

    // m_Vaos.clear();
    // m_Vaos.shrink_to_fit();

    // m_Vbos.clear();
    // m_Vbos.shrink_to_fit();

    // m_Ebos.clear();
    // m_Ebos.shrink_to_fit();

    // m_Vaos.resize(m_Textures.size());
    // m_Vbos.reserve(m_Textures.size());
    // m_Ebos.resize(m_Textures.size());

    // glGenVertexArrays(m_Vaos.size(), m_Vaos.data());
    // glGenBuffers(m_Vbos.size(), m_Vbos.data());
    // glGenBuffers(m_Ebos.size(), m_Ebos.data());

    // for (int i = 0; i < static_cast<int>(m_Textures.size()); ++i)
    // {
    //     int row = i / m_Columns;
    //     int col = i % m_Columns;

    //     float x0 = -1.f + col * m_CellWidth;
    //     float y0 = 1.f - row * m_CellHeight;
    //     float x1 = x0 + m_CellWidth;
    //     float y1 = y0 - m_CellHeight;

    //     float vertices[16] = {
    //         x0, y0, 0.f, 0.f,
    //         x1, y0, 1.f, 0.f,
    //         x1, y1, 1.f, 1.f,
    //         x0, y1, 0.f, 1.f,
    //     };

    //     unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

    //     glBindVertexArray(m_Vaos[i]);

    //     glBindBuffer(GL_ARRAY_BUFFER, m_Vbos[i]);
    //     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebos[i]);
    //     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    //     glEnableVertexAttribArray(0);

    //     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    //     glEnableVertexAttribArray(1);

    //     glBindVertexArray(0);
    // }

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

glm::mat4 GridRenderLayer::ModelViewProjectionMat(float width, float height, float offsetx, float offsety) const
{
    // float viewAspect = (width() * WidthDivisor()) / (height() * HeightDivisor());
    float viewAspect = m_CellWidth / m_CellHeight;
    float imageAspect = width / float((height ? height : 1));

    /* Find the overall scale of the image */
    glm::vec2 scale = (imageAspect > viewAspect) ? glm::vec2(1.f, viewAspect / imageAspect) : glm::vec2(imageAspect / viewAspect, 1.f);

    /**
     * Get the Model matrix,
     * This is how our image/model looks like as a 4x4 matrix
     */
    glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(offsetx, offsety, 0.f));

    /* Update the model with the aspect and the zoom scale */
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.f));

    /**
     * And the projection matrix of how it's supposed to be projected on the viewport
     * Holds the scaling and aspect
     */
    glm::mat4 projection = glm::ortho(-1.f, 1.f, -1.f, 1.f);

    /**
     * Calculate the model view prohection matrix
     * For any 2D Texture/image the transform from the camera is unity
     */
    return model * projection; // * unity view i.e. glm::mat4(1.f)
}

VOID_NAMESPACE_CLOSE
