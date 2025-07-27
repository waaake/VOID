/* GLEW */
#include <GL/glew.h>

/* OpenColor IO */
#include <OpenColorIO/OpenColorIO.h>

/* Internal */
#include "ImageShaderProgram.h"
#include "VoidCore/Logging.h"
#include "VoidCore/ColorProcessor.h"

namespace OCIO = OCIO_NAMESPACE;

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

// /* Base Shader Code */
// std::string fragmentShaderBase = R"(
// #version 330 core
// in vec2 TexCoord;
// out vec4 FragColor;

// uniform sampler2D uTexture;

// // Viewer Proprties
// uniform float exposure;
// uniform float gamma;
// uniform float gain;

// // Channels to display
// uniform int channelMode;
// )";

static const std::string fragmentShaderMain = R"(
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
            FragColor = OCIOFinal(OCIODisplay(vec4(color.r, color.r, color.r, 1.f)));
            break;
        case 1: // Green Channels only
            FragColor = OCIOFinal(OCIODisplay(vec4(color.g, color.g, color.g, 1.f)));
            break;
        case 2: // Blue Channels only
            FragColor = OCIOFinal(OCIODisplay(vec4(color.b, color.b, color.b, 1.f)));
            break;
        case 3: // Only Alpha
            FragColor = OCIOFinal(OCIODisplay(vec4(color.a, color.a, color.a, 1.f)));
            break;
        case 4: // RGB without alpha
            FragColor = OCIOFinal(OCIODisplay(vec4(color.rgb, 1.f)));
            break;
        case 5:
        default: // Show all channels -- default
            FragColor = OCIOFinal(OCIODisplay(color));
    }
}
)";


// static const std::string fragmentShaderSrc = R"(
// #version 330 core
// in vec2 TexCoord;
// out vec4 FragColor;

// uniform sampler2D uTexture;

// // Viewer Proprties
// uniform float exposure;
// uniform float gamma;
// uniform float gain;

// // Channels to display
// uniform int channelMode;

// void main() {
//     // Texture pixel values from the buffers
//     vec4 color = texture(uTexture, TexCoord);

//     // Apply linear gain multiplier
//     color.rgb *= gain;

//     // Apply exposure adjustment
//     // +1 Exposure doubles the light and -1 Exposure halves the light
//     // To mimic that behaviour the rgb is multiplied by 2^exposure
//     color.rgb *= pow(2.f, exposure);

//     // Apply gamma correction
//     color.rgb = pow(color.rgb, vec3(1.0 / gamma));

//     // Render a channel based on the color
//     // 0 = R; 1 = G; 2 = B; 3 = A; 4 = RGB; 5 = RGBA (All)
//     switch (channelMode)
//     {
//         case 0: // Red Channels only 
//             FragColor = vec4(color.r, color.r, color.r, 1.f);
//             break;
//         case 1: // Green Channels only
//             FragColor = vec4(color.g, color.g, color.g, 1.f);
//             break;
//         case 2: // Blue Channels only
//             FragColor = vec4(color.b, color.b, color.b, 1.f);
//             break;
//         case 3: // Only Alpha
//             FragColor = vec4(color.a, color.a, color.a, 1.f);
//             break;
//         case 4: // RGB without alpha
//             FragColor = vec4(color.rgb, 1.f);
//             break;
//         case 5:
//         default: // Show all channels -- default
//             FragColor = color;
//     }
// }
// )";

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
    /* Base Shader Code */
    std::string fragmentShaderBase = R"(
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
    )";

    ColorProcessor& proc = ColorProcessor::Instance();
    proc.SetConfig("/mnt/c/Skids/zework/dev/VOID/resources/nuke-default.ocio");

    /* Load OCIO Config */
    // OCIO::ConstConfigRcPtr config = OCIO::Config::CreateFromFile("/mnt/c/Skids/zework/dev/VOID/resources/cg-config-v3.0.0_aces-v2.0_ocio-v2.4.ocio");
    // OCIO::ConstConfigRcPtr config = OCIO::Config::CreateFromBuiltinConfig("studio-config-v2.2.0_aces-v1.3_ocio-v2.4");
    // OCIO::SetCurrentConfig(config);

    // proc.Create("SLog", "default", "sRGB");
    // int n = config->getNumColorSpaces();

    // for (int i = 0 ; i < n; i++)
    // {
    //     VOID_LOG_INFO(config->getColorSpaceNameByIndex(i));
    // }

    
    // std::vector<std::string> displays = std::move(proc.Displays());
    // std::vector<std::string> views = std::move(proc.Views(displays.at(0)));
    // std::vector<std::string> colorspaces = std::move(proc.Colorspaces());

    // VOID_LOG_INFO("\n\nDISPLAYS\n\n");

    // for (auto display : displays)
    // {
    //     VOID_LOG_INFO(display);
    // }

    // VOID_LOG_INFO("\n\nVIEWS\n\n");

    // for (auto view : views)
    // {
    //     VOID_LOG_INFO(view);
    // }


    // VOID_LOG_INFO("\n\nCOLORSPACES\n\n");

    // for (auto color : colorspaces)
    // {
    //     VOID_LOG_INFO(color);
    // }


    // /* Processor from Linear to sRGB */
    // OCIO::ConstProcessorRcPtr processor = config->getProcessor("ACEScg", "sRGB - Display");

    // /* GPU Processor */
    // OCIO::ConstGPUProcessorRcPtr gpuProcessor = processor->getDefaultGPUProcessor();

    // /* OCIO Shader snippet */
    // OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();
    // shaderDesc->setLanguage(OCIO::GPU_LANGUAGE_GLSL_1_3);
    // shaderDesc->setFunctionName("OCIODisplay");

    // gpuProcessor->extractGpuShaderInfo(shaderDesc);

    // std::string ocioShader = shaderDesc->getShaderText();

    // VOID_LOG_INFO(ocioShader);

    /* Update Shader Code */
    /* Linear */
    // proc.Create();
    proc.Create("sRGB", "scene_linear");
    fragmentShaderBase += std::move(proc.Shader("OCIODisplay"));

    proc.Create("scene_linear", "rec709");
    fragmentShaderBase += std::move(proc.Shader("OCIOFinal"));

    fragmentShaderBase += fragmentShaderMain;

    VOID_LOG_INFO(fragmentShaderBase);

    // fragmentShaderBase 

    /* Add Shaders */
    m_Program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSrc.c_str());
    m_Program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderBase.c_str());

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
