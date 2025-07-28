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

// Input Colorspace
uniform int inputColorSpace;

float Rec709ToLinear(float value)
{
    return (value <= 0.081) ? value / 4.5 : pow((value + 0.099) / 1.099, 1.0 / 0.45);
}

float SRGBToLinear(float value)
{
    return (value <= 0.04045) ? value / 12.92 : pow((value + 0.055) / 1.055, 2.4);
}

vec3 inverseRec709(vec3 c)
{
    vec3 linear;

    linear.r = Rec709ToLinear(c.r);
    linear.g = Rec709ToLinear(c.g);
    linear.b = Rec709ToLinear(c.b);

    return linear;
}

vec3 inverseSRGB(vec3 c)
{
    vec3 linear;

    linear.r = SRGBToLinear(c.r);
    linear.g = SRGBToLinear(c.g);
    linear.b = SRGBToLinear(c.b);

    return linear;
}

// Converts into Linear Colorspace from the input colorspace
// Returns the converted vec4 color
vec4 Linearize(vec4 color, int colorspace)
{
    // No Conversion as the Colorspace is already Linear
    if (colorspace == 0)                                    // Linear
        return color;
    else if (colorspace == 1)                               // Rec.709
        return vec4(inverseRec709(color.rgb), color.a);
    else if (colorspace == 2)                               // standard RGB
        return vec4(inverseSRGB(color.rgb), color.a);

    // Default
    return color;
}

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

    // Ensure we have linear output depending on the input colorspace
    vec4 linear = Linearize(color, inputColorSpace);

    // Render a channel based on the color
    // 0 = R; 1 = G; 2 = B; 3 = A; 4 = RGB; 5 = RGBA (All)
    switch (channelMode)
    {
        case 0: // Red Channels only 
            FragColor = vec4(linear.r, linear.r, linear.r, 1.f);
            break;
        case 1: // Green Channels only
            FragColor = vec4(linear.g, linear.g, linear.g, 1.f);
            break;
        case 2: // Blue Channels only
            FragColor = vec4(linear.b, linear.b, linear.b, 1.f);
            break;
        case 3: // Only Alpha
            FragColor = vec4(linear.a, linear.a, linear.a, 1.f);
            break;
        case 4: // RGB without alpha
            FragColor = vec4(linear.rgb, 1.f);
            break;
        case 5:
        default: // Show all channels -- default
            FragColor = linear;
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
