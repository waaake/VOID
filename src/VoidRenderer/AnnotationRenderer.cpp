/* GLM */
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr

/* Internal */
#include "AnnotationRenderer.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* Shaders for Stroke Rendering */
static const std::string vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 normal;

uniform mat4 uMVP;
uniform float uThickness;

void main() {
    // Offset by half of thickness
    vec2 offset = normal * uThickness * 0.5;

    gl_Position = uMVP * vec4(position + offset, 0.0, 1.0);
}
)";

static const std::string fragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

VoidAnnotationsRenderer::VoidAnnotationsRenderer()
    : m_Drawing(false)
    , m_Color(1.f, 1.f, 1.f)
    , m_Size(4.f)
    , m_DrawType(DrawType::NONE)
    , m_VAO(0)
    , m_VBO(0)
    , m_MVPLoc(-1)
    , m_ColorLoc(-1)
    , m_ThicknessLoc(-1)
{
    m_Annotation = nullptr;
}

VoidAnnotationsRenderer::~VoidAnnotationsRenderer()
{
    if (m_StrokeShader)
    {
        delete m_StrokeShader;
        m_StrokeShader = nullptr;
    }
}

Renderer::SharedAnnotation VoidAnnotationsRenderer::NewAnnotation()
{
    /* Create a new Annotation and return it */
    m_Annotation = std::make_shared<Renderer::Annotation>();
    return m_Annotation;
}

bool VoidAnnotationsRenderer::DrawPoint(const glm::vec2& point)
{
    if (!m_Annotation)
        return false;

    /* The first point to be inserted for the annotation */
    if (!m_Drawing)
    {
        m_LastPoint = point;
        m_Drawing = true;
        return true;
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
    m_Annotation->annotation.push_back(v1);
    m_Annotation->annotation.push_back(v2);
    m_Annotation->annotation.push_back(v3);
    m_Annotation->annotation.push_back(v4);

    // VOID_LOG_INFO("[{0}, {1}]", v1.position.x, v1.position.y);
    // VOID_LOG_INFO("[{0}, {1}]", v2.position.x, v2.position.y);
    // VOID_LOG_INFO("[{0}, {1}]", v3.position.x, v3.position.y);
    // VOID_LOG_INFO("[{0}, {1}]", v4.position.x, v4.position.y);

    /* Update the Last point */
    m_LastPoint = point;

    return true;
}

void VoidAnnotationsRenderer::CommitStroke()
{
    /* If the Annotation hasn't been set */
    if (!m_Annotation)
        return;

    /* If the current Annotation is empty -> Don't add anything */
    if (m_Annotation->annotation.empty())
        return;

    // float thickness2 = 8.0f;
    // glm::vec2 p0 = glm::vec2(100, 100);
    // glm::vec2 p1 = glm::vec2(300, 300);
    // glm::vec2 dir = glm::normalize(p1 - p0);
    // glm::vec2 normal = glm::vec2(-dir.y, dir.x);

    // Renderer::AnnotatedVertex v1 = { p0 + normal * thickness * 0.5f, normal };
    // Renderer::AnnotatedVertex v2 = { p0 - normal * thickness * 0.5f, -normal };
    // Renderer::AnnotatedVertex v3 = { p1 + normal * thickness * 0.5f, normal };
    // Renderer::AnnotatedVertex v4 = { p1 - normal * thickness * 0.5f, -normal };

    // m_Annotation = {v1, v2, v3, v4};
    
    Renderer::Stroke stroke;
    /* Move the annotations here */
    stroke.vertices = std::move(m_Annotation->annotation);
    stroke.color = m_Color;
    stroke.thickness = m_Size;

    m_Annotation->strokes.push_back(std::move(stroke));

    /* Clear the Current Annotation */
    m_Annotation->annotation.clear();
    m_Drawing = false;
}

void VoidAnnotationsRenderer::Initialize()
{
    /* Create the GL Program */
    // m_Shader = new QOpenGLShaderProgram();

    // /* Load the Shader */
    // LoadShaders();

    m_StrokeShader = new StrokeShaderProgram;

    m_StrokeShader->Initialize();

    /* Setup Array Buffers */
    SetupArrays();

    /* Load all the locations for uniforms */
    m_MVPLoc = glGetUniformLocation(m_StrokeShader->ProgramId(), "uMVP");
    m_ColorLoc = glGetUniformLocation(m_StrokeShader->ProgramId(), "uColor");
    m_ThicknessLoc = glGetUniformLocation(m_StrokeShader->ProgramId(), "uThickness");
}

void VoidAnnotationsRenderer::Render(const glm::mat4& projection)
{
    if (!m_Annotation)
        return;

    /* Use the Program */
    // glUseProgram(m_Shader->programId());
    // /* Bind the Shader program */
    // m_Shader->bind();
    m_StrokeShader->Bind();

    // VOID_LOG_INFO("{0}={1}={2}", m_MVPLoc, m_ColorLoc, m_ThicknessLoc);

    /* Update the Projection matrix */
    glUniformMatrix4fv(m_MVPLoc, 1, GL_FALSE, glm::value_ptr(projection));

    /* Bind the Vertex Array */
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    /* Draw out the stokes */
    for (const Renderer::Stroke& stroke: m_Annotation->strokes)
    {
        /* Update uniforms */
        glUniform3fv(m_ColorLoc, 1, glm::value_ptr(stroke.color));
        glUniform1f(m_ThicknessLoc, stroke.thickness);

        /* Update the data on the buffer */
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Renderer::AnnotatedVertex) * stroke.vertices.size(), stroke.vertices.data(), GL_DYNAMIC_DRAW);

        /* Draw the strokes as Triangle strip with stroke thickness */
        glDrawArrays(GL_TRIANGLE_STRIP, 0, stroke.vertices.size());

        // VOID_LOG_INFO("DRAWING...");
        // VOID_LOG_INFO(stroke.vertices.size());
        // VOID_LOG_INFO(stroke.thickness);
    }

    // GLenum err;
    // while ((err = glGetError()) != GL_NO_ERROR) {
    //     // std::cerr << "OpenGL error: " << err << std::endl;
    //     VOID_LOG_ERROR("OPENGL ERROR: {0}", err);
    // }

    if (!m_Annotation->annotation.empty())
    {
        /* Update uniforms */
        glUniform3fv(m_ColorLoc, 1, glm::value_ptr(m_Color));
        glUniform1f(m_ThicknessLoc, m_Size);
    
        /* Update the data on the buffer */
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Renderer::AnnotatedVertex) * m_Annotation->annotation.size(), m_Annotation->annotation.data(), GL_DYNAMIC_DRAW);
    
        /* Draw the strokes as Triangle strip with stroke thickness */
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_Annotation->annotation.size());
    }

    /* Cleanup */
    glBindVertexArray(0);
    // m_Shader->release();
    // glUseProgram(0);
    m_StrokeShader->Release();
}

// bool VoidAnnotationsRenderer::LoadShaders()
// {
//     /* Load the Shader */
//     m_Shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSrc.c_str());
//     m_Shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSrc.c_str());

//     /* If we're not able to link the shaders */
//     if (!m_Shader->link())
//     {
//         VOID_LOG_ERROR("Failed to load Stroke Shaders: {0}", m_Shader->log().toStdString());
//         return false;
//     }

//     /* We're All good */
//     VOID_LOG_INFO("Stroke Shaders Loaded.");
//     return true;
// }

void VoidAnnotationsRenderer::SetupArrays()
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

VOID_NAMESPACE_CLOSE
