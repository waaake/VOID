/* Internal */
#include "RenderTypes.h"
#include "StrokeRenderGear.h"

VOID_NAMESPACE_OPEN

StrokeRenderGear::StrokeRenderGear()
    : m_VAO(0)
    , m_VBO(0)
    , m_UProjection(-1)
    , m_UColor(-1)
    , m_USize(-1)
{
}

StrokeRenderGear::~StrokeRenderGear()
{
    if (m_Shader)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }
}

void StrokeRenderGear::Initialize()
{
    m_Shader = new StrokeShaderProgram;

    /* Initialize the Shaders */
    m_Shader->Initialize();

    /* Initialize the Array Buffers */
    SetupBuffers();

    /* Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UColor = glGetUniformLocation(m_Shader->ProgramId(), "uColor");
    m_USize = glGetUniformLocation(m_Shader->ProgramId(), "uThickness");
}

void StrokeRenderGear::SetupBuffers()
{
    /* Gen Array */
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    /* Bind */
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    /* Positions */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer::AnnotatedVertex), (void*)offsetof(Renderer::AnnotatedVertex, Renderer::AnnotatedVertex::position));
    glEnableVertexAttribArray(0);

    /* Normals */
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer::AnnotatedVertex), (void*)offsetof(Renderer::AnnotatedVertex, Renderer::AnnotatedVertex::normal));
    glEnableVertexAttribArray(1);

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// bool StrokeRenderGear::PreDraw()
// {
//     /* Use the Shader Program */
//     m_Shader->Bind();

//     /* Bind the Vertex Array */
//     glBindVertexArray(m_VAO);
//     glEnableVertexAttribArray(0);
//     glEnableVertexAttribArray(1);

//     return true;
// }

// void StrokeRenderGear::Draw(const RenderKit& data)
// {
//     /* Cast the Data back to Renderable Annotation */
//     // Renderer::SharedRenderableAnnotation d = std::static_pointer_cast<Renderer::RenderableAnnotation>(data.Get());

//     // /* Cannot cast the data */
//     // if (!d)
//     //     return;

//     // /* Update the Projection matrix */
//     // glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(d->projection));

//     // // /* Bind the Vertex Array */
//     // // glBindVertexArray(m_VAO);
//     // // glEnableVertexAttribArray(0);
//     // // glEnableVertexAttribArray(1);

//     // /* Draw out the stokes */
//     // for (const Renderer::Stroke& stroke: d->strokes)
//     // {
//     //     /* Update uniforms */
//     //     glUniform3fv(m_UColor, 1, glm::value_ptr(stroke.color));
//     //     glUniform1f(m_USize, stroke.thickness);

//     //     /* Update the data on the buffer */
//     //     glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//     //     glBufferData(GL_ARRAY_BUFFER, sizeof(Renderer::AnnotatedVertex) * stroke.vertices.size(), stroke.vertices.data(), GL_DYNAMIC_DRAW);

//     //     /* Draw the strokes as Triangle strip with stroke thickness */
//     //     glDrawArrays(GL_TRIANGLE_STRIP, 0, stroke.vertices.size());

//     //     // VOID_LOG_INFO("DRAWING...");
//     //     // VOID_LOG_INFO(stroke.vertices.size());
//     //     // VOID_LOG_INFO(stroke.thickness);
//     // }

//     // // GLenum err;
//     // // while ((err = glGetError()) != GL_NO_ERROR) {
//     // //     // std::cerr << "OpenGL error: " << err << std::endl;
//     // //     VOID_LOG_ERROR("OPENGL ERROR: {0}", err);
//     // // }

//     // if (!d->current.empty())
//     // {
//     //     /* Update uniforms */
//     //     glUniform3fv(m_UColor, 1, glm::value_ptr({1.f, 1.f, 1.f}));
//     //     glUniform1f(m_USize, 4.f);
    
//     //     /* Update the data on the buffer */
//     //     glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//     //     glBufferData(GL_ARRAY_BUFFER, sizeof(Renderer::AnnotatedVertex) * d->current.size(), d->current.data(), GL_DYNAMIC_DRAW);
    
//     //     /* Draw the strokes as Triangle strip with stroke thickness */
//     //     glDrawArrays(GL_TRIANGLE_STRIP, 0, d->current.size());
//     // }
// }

// void StrokeRenderGear::PostDraw()
// {
//     /* Cleanup */
//     glBindVertexArray(0);
//     m_Shader->Release();
// }

VOID_NAMESPACE_CLOSE
