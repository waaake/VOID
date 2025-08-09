// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* Internal */
#include "SwiperShaderProgram.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/**
 * Lines Shaders -- The lines controlling the Actions on the Viewport
 */
static const char* s_VertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 uMVP;

void main() {
    gl_Position = uMVP * vec4(position, 1.0);
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

SwiperShaderProgram::~SwiperShaderProgram()
{
    m_Program->deleteLater();
    delete m_Program;
    m_Program = nullptr;
}

void SwiperShaderProgram::Initialize()
{
    m_Program = new QOpenGLShaderProgram;

    /* Setup the Shaders */
    SetupShaders();
}

bool SwiperShaderProgram::SetupShaders()
{
    /* Add Shaders */
    m_Program->addShaderFromSourceCode(QOpenGLShader::Vertex, s_VertexShaderSrc);
    m_Program->addShaderFromSourceCode(QOpenGLShader::Fragment, s_FragmentShaderSrc);

    /* Try and Compile - Link Shaders */
    if (!m_Program->link())
    {
        /* Log the Errors from the Program */
        VOID_LOG_ERROR("Unable to Link Swipe Shaders: {0}", m_Program->log().toStdString());
        return false;
    }

    /* We're all good */
    VOID_LOG_INFO("Swipe Shaders Loaded.");
    return true;
}

void SwiperShaderProgram::Reinitialize()
{
    /* Unbind */
    Release();

    /* Delete the current Program */
    m_Program->deleteLater();
    delete m_Program;
    m_Program = nullptr;

    /* Reinit */
    Initialize();
}

VOID_NAMESPACE_CLOSE
