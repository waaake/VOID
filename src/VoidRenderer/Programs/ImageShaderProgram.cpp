/* GLEW */
#include <GL/glew.h>

/* Internal */
#include "ImageShaderProgram.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/**
 * These are our base shaders which we'll use to render out images
 * onto our renderer window
 */

static const std::string vertexShaderSrc = R"(
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

static const std::string fragmentShaderSrc = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

// Viewer Proprties
uniform float exposure;
uniform float gamma;
uniform float gain;

// Channels to display
uniform int channelMode;

void main() {
    // Texture pixel values from the buffers
    vec4 color = texture(uTexture, TexCoord);

    // Apply linear gain multiplier
    color.rgb *= gain;

    // Apply exposure adjustment
    // +1 Exposure doubles the light and -1 Exposure halves the light
    // To mimic that behaviour the rgb is multiplied by 2^exposure
    color.rgb *= pow(2.f, exposure);

    // Apply gamma correction
    color.rgb = pow(color.rgb, vec3(1.0 / gamma));

    // Render a channel based on the color
    // 0 = R; 1 = G; 2 = B; 3 = A; 4 = RGB; 5 = RGBA (All)
    switch (channelMode)
    {
        case 0: // Red Channels only 
            FragColor = vec4(color.r, color.r, color.r, 1.f);
            break;
        case 1: // Green Channels only
            FragColor = vec4(color.g, color.g, color.g, 1.f);
            break;
        case 2: // Blue Channels only
            FragColor = vec4(color.b, color.b, color.b, 1.f);
            break;
        case 3: // Only Alpha
            FragColor = vec4(color.a, color.a, color.a, 1.f);
            break;
        case 4: // RGB without alpha
            FragColor = vec4(color.rgb, 1.f);
            break;
        case 5:
        default: // Show all channels -- default
            FragColor = color;
    }
}
)";

ImageShaderProgram::~ImageShaderProgram()
{
    m_Program->deleteLater();
    delete m_Program;
    m_Program = nullptr;
}

void ImageShaderProgram::Initialize()
{
    m_Program = new QOpenGLShaderProgram;

    /* Setup the Shaders */
    SetupShaders();
}

bool ImageShaderProgram::SetupShaders()
{
    /* Add Shaders */
    m_Program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSrc.c_str());
    m_Program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSrc.c_str());

    /* Try and Compile - Link Shaders */
    if (!m_Program->link())
    {
        /* Log the Errors from the Program */
        VOID_LOG_ERROR("Unable to Link Image Shaders: {0}", m_Program->log().toStdString());
        return false;
    }

    /* We're all good */
    VOID_LOG_INFO("Image Shaders Loaded.");
    return true;
}

VOID_NAMESPACE_CLOSE
