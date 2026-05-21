// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "GridShaderProgram.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

static const char* s_VertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 v_TexCoord;

uniform mat4 uMVP;
out vec2 TexCoord;

void main() {
    gl_Position = uMVP * vec4(position, 0.0, 1.0);
    TexCoord = v_TexCoord;
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    FragColor = texture(uTexture, TexCoord);
}
)";

GridShaderProgram::~GridShaderProgram()
{
    if (m_Program)
    {
        m_Program->deleteLater();
        delete m_Program;
        m_Program = nullptr;
    }
}

void GridShaderProgram::Initialize()
{
    m_Program = new QOpenGLShaderProgram;
    SetupShaders();
}

bool GridShaderProgram::SetupShaders()
{
    m_Program->addShaderFromSourceCode(QOpenGLShader::Vertex, s_VertexShaderSrc);
    m_Program->addShaderFromSourceCode(QOpenGLShader::Fragment, s_FragmentShaderSrc);

    if (!m_Program->link())
    {
        VOID_LOG_ERROR("Failed to Link Grid Shaders: {}", m_Program->log().toStdString());
        return false;
    }

    VOID_LOG_INFO("Grid Shaders Loaded");
    return true;
}

void GridShaderProgram::Reinitialize()
{
    Release();

    m_Program->deleteLater();
    delete m_Program;
    m_Program = nullptr;

    Initialize();
}

VOID_NAMESPACE_CLOSE
