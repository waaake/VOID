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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer::AnnotatedVertex), nullptr);
    glEnableVertexAttribArray(0);

    /* Normals */
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer::AnnotatedVertex), nullptr);
    glEnableVertexAttribArray(1);

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool StrokeRenderGear::PreDraw()
{
    /* Use the Shader Program */
    m_Shader->Bind();

    /* Bind the Vertex Array */
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return true;
}

void StrokeRenderGear::Draw(const void* data)
{
    /* Cast the Data back to Renderable Annotation */
    const Renderer::AnnotationRenderData* d = static_cast<const Renderer::AnnotationRenderData*>(data);

    /* Cannot cast the data */
    if (!d)
        return;

    /* Update the Projection matrix */
    glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(d->projection));

    /* Draw out the existing stokes */
    for (const Renderer::Stroke& stroke: d->annotation->strokes)
    {
        /* Update uniforms */
        glUniform3fv(m_UColor, 1, glm::value_ptr(stroke.color));
        glUniform1f(m_USize, stroke.thickness);

        /* Update the data on the buffer */
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Renderer::AnnotatedVertex) * stroke.Size(), stroke.Data(), GL_DYNAMIC_DRAW);

        /* Draw the strokes as Triangle strip with stroke thickness */
        glDrawArrays(GL_TRIANGLE_STRIP, 0, stroke.Size());
    }

    /* Draw out the */
    if (!d->annotation->current.Empty())
    {
        /* Update uniforms */
        glUniform3fv(m_UColor, 1, glm::value_ptr(d->annotation->current.color));
        glUniform1f(m_USize, d->annotation->current.thickness);
    
        /* Update the data on the buffer */
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Renderer::AnnotatedVertex) * d->annotation->current.Size(), d->annotation->current.Data(), GL_DYNAMIC_DRAW);
    
        /* Draw the strokes as Triangle strip with stroke thickness */
        glDrawArrays(GL_TRIANGLE_STRIP, 0, d->annotation->current.Size());
    }
}

void StrokeRenderGear::PostDraw()
{
    /* Cleanup */
    glBindVertexArray(0);
    m_Shader->Release();
}

VOID_NAMESPACE_CLOSE
