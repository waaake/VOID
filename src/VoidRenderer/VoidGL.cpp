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
uniform sampler2D uTextureB;

// Viewer Proprties
uniform float exposure;
uniform float gamma;
uniform float gain;

// Channels to display
uniform int channelMode;

// Comparison
uniform int comparisonMode;
uniform int blendMode;

// Swipe
uniform float swipeX;

void main() {
    // Out Color
    vec4 color = vec4(1.f);

    // Texture pixel values from the buffers
    vec4 colorA = texture(uTexture, TexCoord);
    vec4 colorB = texture(uTextureB, TexCoord);

    // Clamped swipe to Texture Coordinates
    float swipe = clamp(swipeX, 0.0, 1.0);

    // The Comparison mode Decides if we're displaying A Buffer or B or a Swipe Comparison
    // 0 indicates No Comparison (OFF state for Compare Mode)
    // 1 indicates Wipe (The swipe determines what pixels to read from A and what from B)
    // 2 indicates Stack (full pixels be it from A or B)
    if (comparisonMode == 0)
    {
        // The Color will always be the Primary Buffer (A or B in renderer terms)
        color = colorA;
    }
    else if (comparisonMode == 1)
    {
        // Outcolor considers blend mode
        // Two Blend Modes work with Wipe
        switch (blendMode)
        {
            case 0: // A Under B
                if (TexCoord.x < swipe)
                    color = colorB;
                else
                    color = colorA;
    
                break;
            case 1: // A Over B
            default:
                if (TexCoord.x < swipe)
                    color = colorA;
                else
                    color = colorB;
        }
    }
    else
    {
        // Outcolor based on blend mode
        switch (blendMode)
        {
            case 0: // A Under B
                color = colorB;
                break;
            case 1: // A Over B
                color = colorA;
                break;
            case 2:
                color = colorA + colorB;
                break;
            case 3:
                color = mix(colorA, colorB, 0.5);
                break;
            default:
                color = colorA; 
        }
    }

    // By Now we know the pixel color which will be finally rendered
    // Next are the adjustments for exposure and color

    // // Get the outcolor based on the comparison mode
    // switch (comparisonMode)
    // {
    //     case 0:
    //         color = colorA;
    //         break;
    //     case 1:
    //         // color = colorB;
    //         if (TexCoord.x < swipe)
    //             color = colorB;
    //         else
    //             color = colorA;
    //         break;
    //     case 2:
    //         color = colorA + colorB;
    //         break;
    //     case 3:
    //         color = colorA - colorB;
    //         break;
    //     case 4:
    //         color = mix(colorA, colorB, 0.5);
    //         break;
    //     default:
    //         color = colorA;
    // }

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


/* Lines Shaders -- The lines controlling the Actions on the Viewport {{{ */
static const std::string lineVertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 uMVP;

void main() {
    gl_Position = uMVP * vec4(position, 1.0);
}
)";

static const std::string lineFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

/* }}} */


VoidShader::VoidShader()
{
}

VoidShader::~VoidShader()
{
    if (m_Shader)
        m_Shader->deleteLater();
}

void VoidShader::Initialize()
{
    /* Construct a program to be used */
    m_Shader = new QOpenGLShaderProgram;
    m_SwipeShader = new QOpenGLShaderProgram;

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
    LoadSwipeShaders();
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

bool VoidShader::LoadSwipeShaders()
{
    /* Compile and Link the Swipe Shaders */
    m_SwipeShader->addShaderFromSourceCode(QOpenGLShader::Vertex, lineVertexShaderSrc.c_str());
    m_SwipeShader->addShaderFromSourceCode(QOpenGLShader::Fragment, lineFragmentShaderSrc.c_str());

    /* If We're not able to link the swipe shaders */
    if (!m_SwipeShader->link())
    {
        /* Log the error */
        VOID_LOG_ERROR("Swipe Shader Linking Failed: {0}", m_SwipeShader->log().toStdString());
        return false;
    }

    /* All Good */
    VOID_LOG_INFO("Swipe Shaders Loaded.");
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
