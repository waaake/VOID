// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QFile>

/* GLM */
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr

/* Internal */
#include "StrokeRenderLayer.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

StrokeRenderLayer::StrokeRenderLayer()
    : m_Annotation(nullptr)
    , m_Shader(nullptr)
    , m_VAO(0)
    , m_VBO(0)
    , m_UProjection(-1)
    , m_UColor(-1)
    , m_USize(-1)
    , m_Color(1.f, 1.f, 1.f)
    , m_Drawing(false)
    , m_Size(0.004f)
{
}

StrokeRenderLayer::~StrokeRenderLayer()
{
    if (m_Shader)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }
}

void StrokeRenderLayer::DrawPoint(const glm::vec2& point)
{
    if (!m_Annotation)
        return;

    /* The first point to be inserted for the annotation */
    if (!m_Drawing)
    {
        m_LastPoint = point;
        m_Drawing = true;

        /* Initialize the Stroke with the Color and Size */
        m_Annotation->current.color = m_Color;
        m_Annotation->current.thickness = m_Size;

        return;
    }

    /* Next Set of Points */
    glm::vec2 direction = glm::normalize(point - m_LastPoint);
    glm::vec2 normal = glm::vec2(-direction.y, direction.x);

    float halfThickness = m_Size * 0.5f;

    /* The Annotated Vertex */
    Renderer::AnnotatedVertex v1 = { m_LastPoint + normal * halfThickness, normal};
    Renderer::AnnotatedVertex v2 = { m_LastPoint - normal * halfThickness, -normal};
    Renderer::AnnotatedVertex v3 = { point + normal * halfThickness, normal};
    Renderer::AnnotatedVertex v4 = { point - normal * halfThickness, -normal};

    /* Add to the Set of Vertices */
    m_Annotation->current.vertices.push_back(v1);
    m_Annotation->current.vertices.push_back(v2);
    m_Annotation->current.vertices.push_back(v3);
    m_Annotation->current.vertices.push_back(v4);

    /* Update the Last point */
    m_LastPoint = point;

    return;
}

void StrokeRenderLayer::EraseStroke(const glm::vec2& point)
{
    /* No Active Annotation */
    if (!m_Annotation)
        return;

    auto colliding = [&](const Renderer::Stroke& stroke) -> bool 
    {
        for (const Renderer::AnnotatedVertex& vertex: stroke.vertices)
        {
            if (glm::distance(vertex.position, point) < 0.05)   // If the point is closer than the threshold
                return true;
        }

        /* Not Colliding */
        return false;
    };

    /* Erase if the point collides on the stroke */
    m_Annotation->strokes.erase(std::remove_if(m_Annotation->strokes.begin(), m_Annotation->strokes.end(), colliding), m_Annotation->strokes.end());
}

void StrokeRenderLayer::CommitStroke()
{
    /* If the Annotation hasn't been set */
    if (!m_Annotation)
        return;

    /* If the current Annotation is empty -> Don't add anything */
    if (m_Annotation->current.Empty())
        return;

    /* Move the Current annotation to the collection of strokes */
    m_Annotation->strokes.push_back(std::move(m_Annotation->current));

    /* Clear the Current Annotation */
    m_Annotation->current.Clear();
    m_Drawing = false;
}

void StrokeRenderLayer::Initialize()
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

void StrokeRenderLayer::Render(const glm::mat4& projection)
{
    m_Projection = projection;
    
    if (PreDraw())
        Draw();

    PostDraw();
}

void StrokeRenderLayer::SetupBuffers()
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

bool StrokeRenderLayer::PreDraw()
{
    /* Use the Shader Program */
    m_Shader->Bind();

    /* Bind the Vertex Array */
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    /* Enable MultiSample to allow smoothness of annotated lines */
    glEnable(GL_MULTISAMPLE);

    return true;
}

void StrokeRenderLayer::Draw()
{
    /* Update the Projection matrix */
    glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(m_Projection));

    /* Draw out the existing stokes */
    for (const Renderer::Stroke& stroke: m_Annotation->strokes)
        DrawStroke(stroke);

    /* Draw out the current stroke */
    if (!m_Annotation->current.Empty())
        DrawStroke(m_Annotation->current);
}

void StrokeRenderLayer::DrawStroke(const Renderer::Stroke& stroke)
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

void StrokeRenderLayer::PostDraw()
{
    /* Done smoothening */
    glDisable(GL_MULTISAMPLE);

    /* Cleanup */
    glBindVertexArray(0);
    m_Shader->Release();
}

VOID_NAMESPACE_CLOSE
