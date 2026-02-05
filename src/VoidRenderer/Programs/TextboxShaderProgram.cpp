// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* Internal */
#include "TextboxShaderProgram.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/**
 * Shaders to be used for Textbox Rendering
 */
static const char* s_VertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec2 position;

out vec2 FragPos;

uniform mat4 uMVP;

void main() {
    FragPos = position;
    gl_Position = uMVP * vec4(position, 0.0, 1.0);
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 330 core
in vec2 FragPos;
out vec4 FragColor;

uniform vec2 uMin;
uniform vec2 uMax;
uniform vec3 uColor;
uniform float uThickness;

void main() {
    bool border = (FragPos.x <= uMin.x + uThickness || FragPos.x >= uMax.x - uThickness ||
                   FragPos.y <= uMin.y + uThickness || FragPos.y >= uMax.y - uThickness);

    if (border)
        FragColor = vec4(uColor, 1.f);
    else
        discard;
}
)";

TextBoxShaderProgram::~TextBoxShaderProgram()
{
    m_Program->deleteLater();
    delete m_Program;
    m_Program = nullptr;
}

void TextBoxShaderProgram::Initialize()
{
    m_Program = new QOpenGLShaderProgram;

    /* Setup the Shaders */
    SetupShaders();
}

bool TextBoxShaderProgram::SetupShaders()
{
    /* Add Shaders */
    m_Program->addShaderFromSourceCode(QOpenGLShader::Vertex, s_VertexShaderSrc);
    m_Program->addShaderFromSourceCode(QOpenGLShader::Fragment, s_FragmentShaderSrc);

    /* Try and Compile - Link Shaders */
    if (!m_Program->link())
    {
        /* Log the Errors from the Program */
        VOID_LOG_ERROR("Unable to Link Textbox Shaders: {0}", m_Program->log().toStdString());
        return false;
    }

    /* We're all good */
    VOID_LOG_INFO("Textbox Shaders Loaded.");
    return true;
}

void TextBoxShaderProgram::Reinitialize()
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
