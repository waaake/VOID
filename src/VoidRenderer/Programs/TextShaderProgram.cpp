// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* Internal */
#include "TextShaderProgram.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/**
 * Shaders to be used for Text Rendering
 */
static const char* s_VertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 vTexCoord;
out vec2 TexCoord;

uniform mat4 uMVP;
uniform float uThickness;

void main() {
    gl_Position = uMVP * vec4(position, 0.0, 1.0);
    TexCoord = vTexCoord;
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uText;
uniform vec3 uColor;

void main() {
    // Alpha is used for blending to create smooth edges
    // Samples red channel of the glyph texture
    float alpha = texture(uText, TexCoord).r;

    FragColor = vec4(uColor, alpha);
}
)";

TextShaderProgram::~TextShaderProgram()
{
    m_Program->deleteLater();
    delete m_Program;
    m_Program = nullptr;
}

void TextShaderProgram::Initialize()
{
    m_Program = new QOpenGLShaderProgram;

    /* Setup the Shaders */
    SetupShaders();
}

bool TextShaderProgram::SetupShaders()
{
    /* Add Shaders */
    m_Program->addShaderFromSourceCode(QOpenGLShader::Vertex, s_VertexShaderSrc);
    m_Program->addShaderFromSourceCode(QOpenGLShader::Fragment, s_FragmentShaderSrc);

    /* Try and Compile - Link Shaders */
    if (!m_Program->link())
    {
        /* Log the Errors from the Program */
        VOID_LOG_ERROR("Unable to Link Text Shaders: {0}", m_Program->log().toStdString());
        return false;
    }

    /* We're all good */
    VOID_LOG_INFO("Text Shaders Loaded.");
    return true;
}

void TextShaderProgram::Reinitialize()
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
