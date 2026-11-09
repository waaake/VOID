// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ColorProcessor.h"
#include "VoidCore/Logging.h"

namespace OCIO = OCIO_NAMESPACE;

VOID_NAMESPACE_OPEN

ColorProcessor::ColorProcessor()
{
    SetConfig(Config::Builtin);
}

ColorProcessor& ColorProcessor::Instance()
{
    static ColorProcessor processor;
    return processor;
}

void ColorProcessor::SetConfig(const Config& type)
{
    if (type == Config::Builtin)
        m_Config = OCIO::Config::CreateFromBuiltinConfig("cg-config-v1.0.0_aces-v1.3_ocio-v2.1");
    else
        m_Config = OCIO::Config::CreateFromEnv();
    OCIO::SetCurrentConfig(m_Config);
}

void ColorProcessor::SetConfig(const std::string& path)
{
    m_Config = OCIO::Config::CreateFromFile(path.c_str());
    OCIO::SetCurrentConfig(m_Config);
}

void ColorProcessor::SetConfig(std::istream& stream)
{
    m_Config = OCIO::Config::CreateFromStream(stream);

    OCIO::SetCurrentConfig(m_Config);
}

std::vector<std::string> ColorProcessor::Displays() const
{
    std::vector<std::string> displays;
    int num = m_Config->getNumDisplays();
    displays.reserve(num);

    for (int i = 0; i < num; ++i)
        displays.emplace_back(m_Config->getDisplay(i));

    return displays;
}

std::vector<std::string> ColorProcessor::Views(const std::string& display) const
{
    std::vector<std::string> views;
    int num = m_Config->getNumViews(display.c_str());
    views.reserve(num);

    for (int i = 0; i < num; ++i)
        views.emplace_back(m_Config->getView(display.c_str(), i));

    return views;
}

std::vector<std::string> ColorProcessor::Colorspaces() const
{
    std::vector<std::string> colorspaces;
    int num = m_Config->getNumColorSpaces();
    colorspaces.reserve(num);

    for (int i = 0; i < num; ++i)
        colorspaces.emplace_back(m_Config->getColorSpaceNameByIndex(i));

    return colorspaces;
}

void ColorProcessor::Create()
{
    const char* display = m_Config->getDefaultDisplay();
    const char* view = m_Config->getDefaultView(display);

    OCIO::ConstProcessorRcPtr processor = m_Config->getProcessor(OCIO::ROLE_DEFAULT, display, view, OCIO::TRANSFORM_DIR_FORWARD);
    m_GProcessor = processor->getDefaultGPUProcessor();
}

void ColorProcessor::Create(const std::string& display)
{
    const char* view = m_Config->getDefaultView(display.c_str());

    OCIO::ConstProcessorRcPtr processor = m_Config->getProcessor(OCIO::ROLE_SCENE_LINEAR, display.c_str(), view, OCIO::TRANSFORM_DIR_FORWARD);
    m_GProcessor = processor->getDefaultGPUProcessor();
}

void ColorProcessor::Create(const std::string& source, const std::string& destination)
{
    OCIO::ConstProcessorRcPtr processor = m_Config->getProcessor(source.c_str(), destination.c_str());
    m_GProcessor = processor->getDefaultGPUProcessor();
}

void ColorProcessor::Create(const std::string& inputcolorspace, const std::string& display, const std::string& view)
{
    OCIO::ConstProcessorRcPtr processor = m_Config->getProcessor(inputcolorspace.c_str(), display.c_str(), view.c_str(), OCIO::TRANSFORM_DIR_FORWARD);
    m_GProcessor = processor->getDefaultGPUProcessor();
}

void ColorProcessor::Set(const std::string& display)
{
    Create(OCIO_NAMESPACE::ROLE_SCENE_LINEAR, display);
}

void ColorProcessor::Set(const std::string& display, const std::string& view)
{
    Create(OCIO_NAMESPACE::ROLE_SCENE_LINEAR, display, view);
}

void ColorProcessor::ResetDisplay()
{
    Create(OCIO_NAMESPACE::ROLE_SCENE_LINEAR, m_Config->getDefaultDisplay());
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

void ColorProcessor::ProcessImage(float* pixels, int width, int height, int channels, const std::string& outcolorspace) const
{
    OCIO::ConstProcessorRcPtr processor = m_Config->getProcessor(OCIO::ROLE_SCENE_LINEAR, outcolorspace.c_str());
    OCIO::ConstCPUProcessorRcPtr cpuproc = processor->getDefaultCPUProcessor();

    OCIO::PackedImageDesc imgdesc(pixels, width, height, channels);
    cpuproc->apply(imgdesc);
}

// void ColorProcessor::ProcessImage(float* pixels, int width, int height, int channels, const ColorSpace& colorspace, const std::string& outcolorspace) const
// {
//     OCIO::ConstProcessorRcPtr processor = nullptr;
//     switch (colorspace)
//     {
//         case ColorSpace::Linear:
//             processor = m_Config->getProcessor(OCIO::ROLE_SCENE_LINEAR, outcolorspace.c_str());
//             break;
//         case ColorSpace::sRGB:
//             processor = m_Config->getProcessor("None", outcolorspace.c_str());
//             break;
//         default:
//             processor = m_Config->getProcessor(OCIO::ROLE_DEFAULT, outcolorspace.c_str());
//     }

//     OCIO::ConstCPUProcessorRcPtr cpuproc = processor->getDefaultCPUProcessor();

//     OCIO::PackedImageDesc imgdesc(pixels, width, height, channels);
//     cpuproc->apply(imgdesc);
// }

/// MediaColorProcessor

MediaColorProcessor::ProcessorEntry::ProcessorEntry(const MediaColor& color, const OCIO::ConstConfigRcPtr& config)
    : color(color)
{
    if (color.colorspace == ColorSpaceType::Custom && !color.custom.empty())
        processor = config->getProcessor(color.custom.c_str(), OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::LinearSRGB || color.colorspace == ColorSpaceType::LinearRec709)
        processor = config->getProcessor("Linear Rec.709 (sRGB)", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::Rec709) // Need to handle transfer function with Gamma 2.2 and Gamma 2.4
        processor = config->getProcessor("Camera Rec.709", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::Rec2020) // Linear, need to test this
        processor = config->getProcessor("Linear Rec.2020", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::ACES2065_1)
        processor = config->getProcessor("ACES2065-1", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::ACEScg)
        processor = config->getProcessor("ACEScg", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::ARRI_LogC3)
        processor = config->getProcessor("ARRI LogC3 (EI800)", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::ARRI_LogC4)
        processor = config->getProcessor("ARRI LogC4", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::Sony_SLog3)
        processor = config->getProcessor("S-Log3 S-Gamut3.Cine", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::Panasonic_VLog)
        processor = config->getProcessor("V-Log V-Gamut", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::Canon_Log3)
        processor = config->getProcessor("CanonLog3 CinemaGamut D55", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::ST2084_PQ)
        processor = config->getProcessor("ST2084-P3-D65 - Display", OCIO::ROLE_SCENE_LINEAR);
    else if (color.colorspace == ColorSpaceType::HLG)
        processor = config->getProcessor("Rec.2100-HLG - Display", OCIO::ROLE_SCENE_LINEAR);
    else /// For now, going with SRGB as default
        processor = config->getProcessor("sRGB - Texture", OCIO::ROLE_SCENE_LINEAR);
}

MediaColorProcessor::MediaColorProcessor()
    : m_Processor(nullptr)
{
    m_Config = OCIO::Config::CreateFromBuiltinConfig("studio-config-v1.0.0_aces-v1.3_ocio-v2.1");
}

MediaColorProcessor& MediaColorProcessor::Instance()
{
    static MediaColorProcessor instance;
    return instance;
}

std::vector<std::string> MediaColorProcessor::Colorspaces() const
{
    std::vector<std::string> colorspaces;
    int num = m_Config->getNumColorSpaces();
    colorspaces.reserve(num);

    for (int i = 0; i < num; ++i)
        colorspaces.emplace_back(m_Config->getColorSpaceNameByIndex(i));

    return colorspaces;
}

void MediaColorProcessor::Process(const MediaColor& color, const FloatImage& image)
{
    OCIO::ConstProcessorRcPtr processor = Processor(color);

    OCIO::PackedImageDesc desc(image->Writable(), image->width, image->height, image->channels);
    processor->getDefaultCPUProcessor()->apply(desc);
    // processor->getOptimizedCPUProcessor(OCIO::OptimizationFlags::OPTIMIZATION_LOSSLESS)->apply(desc);
}

OCIO::ConstProcessorRcPtr MediaColorProcessor::Processor(const MediaColor& color)
{
    if (m_Processor && m_Processor->color == color)
        return m_Processor->processor;

    /// Check if we have that color processor already constructred
    for (auto& processor : m_Processors)
    {
        if (processor.color == color)
        {
            m_Processor = &processor;
            return m_Processor->processor;
        }
    }

    /// We don't have it constructed before
    m_Processors.emplace_back(color, m_Config);
    m_Processor = &m_Processors.back();

    return m_Processor->processor;
}

VOID_NAMESPACE_CLOSE
