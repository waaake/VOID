/* Internal */
#include "ColorProcessor.h"
#include "Logging.h"

namespace OCIO = OCIO_NAMESPACE;

VOID_NAMESPACE_OPEN

ColorProcessor::ColorProcessor()
{
    /* Initiate with a Builtin Config */
    SetConfig(Config::Builtin);
}

ColorProcessor& ColorProcessor::Instance()
{
    static ColorProcessor processor;
    return processor;
}

void ColorProcessor::SetConfig(const Config& type)
{
    // m_Config = OCIO::Config::CreateFromBuiltinConfig("studio-config-v2.2.0_aces-v1.3_ocio-v2.4");
    // m_Config = OCIO::Config::CreateFromBuiltinConfig(OCIO::BuiltinConfigRegistry::Get().getBuiltinConfig(1));
    if (type == Config::Builtin)
        m_Config = OCIO::Config::CreateFromBuiltinConfig("ocio://default");
    else
        m_Config = OCIO::Config::CreateFromEnv();

    /* Set the Color Config */
    OCIO::SetCurrentConfig(m_Config);
}

void ColorProcessor::SetConfig(const std::string& path)
{
    m_Config = OCIO::Config::CreateFromFile(path.c_str());

    /* Set the Color Config */
    OCIO::SetCurrentConfig(m_Config);
}

std::vector<std::string> ColorProcessor::Displays() const
{
    std::vector<std::string> displays;
    
    /* Number of Displays */
    int num = m_Config->getNumDisplays();
    displays.reserve(num);

    for (int i = 0; i < num; ++i)
    {
        displays.emplace_back(m_Config->getDisplay(i));
    }

    return displays;
}

std::vector<std::string> ColorProcessor::Views(const std::string& display) const
{
    std::vector<std::string> views;
    
    /* Number of Views */
    int num = m_Config->getNumViews(display.c_str());
    views.reserve(num);

    for (int i = 0; i < num; ++i)
    {
        views.emplace_back(m_Config->getView(display.c_str(), i));
    }

    return views;
}

std::vector<std::string> ColorProcessor::Colorspaces() const
{
    std::vector<std::string> colospaces;
    
    /* Number of Colorspaces */
    int num = m_Config->getNumColorSpaces();
    colospaces.reserve(num);

    for (int i = 0; i < num; ++i)
    {
        colospaces.emplace_back(m_Config->getColorSpaceNameByIndex(i));
    }

    return colospaces;
}

void ColorProcessor::Create()
{
    const char* display = m_Config->getDefaultDisplay();
    // const char* view = m_Config->getDefaultView(display);
    const char* view = "scene_linear";

    VOID_LOG_INFO("{0}::{1}::{2}", OCIO::ROLE_DEFAULT, display, view);

    OCIO::ConstProcessorRcPtr processor = m_Config->getProcessor(OCIO::ROLE_DEFAULT, display, view, OCIO::TRANSFORM_DIR_FORWARD);

    /* Get the GPU processor */
    m_GProcessor = processor->getDefaultGPUProcessor();
}

void ColorProcessor::Create(const std::string& display)
{
    const char* view = m_Config->getDefaultView(display.c_str());
    // const char* view = "Un-tone-mapped";

    VOID_LOG_INFO("DISPLAY->{0}", display);
    VOID_LOG_INFO("VIEW->{0}", view);

    OCIO::ConstProcessorRcPtr processor = m_Config->getProcessor(OCIO::ROLE_SCENE_LINEAR, display.c_str(), view, OCIO::TRANSFORM_DIR_FORWARD);

    /* Get the GPU Processor */
    m_GProcessor = processor->getDefaultGPUProcessor();
}

void ColorProcessor::Create(const std::string& source, const std::string& destination)
{
    OCIO::ConstProcessorRcPtr processor = m_Config->getProcessor(source.c_str(), destination.c_str());

    /* Get the GPU Processor */
    m_GProcessor = processor->getDefaultGPUProcessor();
}

void ColorProcessor::Create(const std::string& inputcolorspace, const std::string& display, const std::string& view)
{
    OCIO::ConstProcessorRcPtr processor = m_Config->getProcessor(inputcolorspace.c_str(), display.c_str(), view.c_str(), OCIO::TRANSFORM_DIR_FORWARD);

    /* Get the GPU processor */
    m_GProcessor = processor->getDefaultGPUProcessor();
}

std::string ColorProcessor::Shader(const std::string& function) const
{
    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();
    shaderDesc->setLanguage(OCIO::GPU_LANGUAGE_GLSL_1_3);
    shaderDesc->setFunctionName(function.c_str());

    /* Get the gpu shader info */
    m_GProcessor->extractGpuShaderInfo(shaderDesc);

    /* The Fragment Shader Code */
    return shaderDesc->getShaderText();
}

VOID_NAMESPACE_CLOSE
