/* Qt */
#include <QSurfaceFormat>

/* Internal */
#include "VoidGL.h"

VOID_NAMESPACE_OPEN

/**
 * These are our base shaders which we'll use to render out images (for now)
 * onto our renderer window
 */

static const std::string vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 v_TexCoord;

uniform mat4 uMVP;
out vec2 TexCoord;

void main() {
    gl_Position = uMVP * vec4(position, 0.0, 1.0);
    TexCoord = v_TexCoord;
}
)";

static const std::string fragmentShaderSrc = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

// Viewer Proprties
uniform float exposure;
uniform float gamma;
uniform float gain;

void main() {
    vec4 color = texture(uTexture, TexCoord);

    // Apply linear gain multiplier
    color.rgb *= gain;

    // Apply exposure adjustment
    color.rgb = vec3(1.0) - exp(-color.rgb * exposure);

    // Apply gamma correction
    color.rgb = pow(color.rgb, vec3(1.0 / gamma));

    FragColor = color;
}
)";


VoidShader::VoidShader()
{
    /* Construct a program to be used */
    m_Shader = new QOpenGLShaderProgram;
}

VoidShader::~VoidShader()
{
    m_Shader->deleteLater();
}

void VoidShader::Initialize()
{
    /* Try and Initialize Glew */
    unsigned int status = glewInit();

    if (status != GLEW_OK)
    {
        VOID_LOG_ERROR("GLEW init Failed: {0}", reinterpret_cast<const char*>(glewGetErrorString(status)));
    }

    /* All good for GL and GLEW */
    VOID_LOG_INFO("GLEW Initialized.");

    /* Load the shaders */
    LoadShaders();
}

bool VoidShader::LoadShaders()
{
    /* Compile Vertex Shader */
    m_Shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSrc.c_str());

    /* Compile Fragment Shader */
    m_Shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSrc.c_str());

    /* If we're not able to link the shaders */
    if (!m_Shader->link())
    {
        /* Log the error and return the status as false */
        VOID_LOG_ERROR("Shader Linking Failed: {0}", m_Shader->log().toStdString());
        return false;
    }

    /* We're all goood */
    VOID_LOG_INFO("Shaders Loaded.");
    return true;
}

void VoidShader::SetProfile()
{
    /**
     * As we're going to use Modern OpenGL
     * We'd like to use the Core Profile
     * Setup OpenGL Core Profile
     */
    QSurfaceFormat format;
    /**
     * The reason we're going for 3,3 is that whatever we need is available in 3,3 there isn't too much of a benefit
     * going up? maybe we do it later?
     */
    format.setVersion(3, 3);    // This is so that our shader gets compiled and linked version 330 core for OpenGL 3.3
    format.setProfile(QSurfaceFormat::CoreProfile);

    /* Set the adjusted profile */
    QSurfaceFormat::setDefaultFormat(format);
}

VOID_NAMESPACE_CLOSE
