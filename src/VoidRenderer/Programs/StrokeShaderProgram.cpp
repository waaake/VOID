/* Internal */
#include "StrokeShaderProgram.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/**
 * Shaders to be used for Stroke Rendering
 */
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

StrokeShaderProgram::~StrokeShaderProgram()
{
    m_Program->deleteLater();
    delete m_Program;
    m_Program = nullptr;
}

void StrokeShaderProgram::Initialize()
{
    m_Program = new QOpenGLShaderProgram;

    /* Setup the Shaders */
    SetupShaders();
}

bool StrokeShaderProgram::SetupShaders()
{
    /* Add Shaders */
    m_Program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSrc.c_str());
    m_Program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSrc.c_str());

    /* Try and Compile - Link Shaders */
    if (!m_Program->link())
    {
        /* Log the Errors from the Program */
        VOID_LOG_ERROR("Unable to Link Stroke Shaders: {0}", m_Program->log().toStdString());
        return false;
    }

    /* We're all good */
    VOID_LOG_INFO("Stroke Shaders Loaded.");
    return true;
}

VOID_NAMESPACE_CLOSE
