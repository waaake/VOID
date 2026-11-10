// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_COLOR_PROCESSOR_H
#define _VOID_COLOR_PROCESSOR_H

/* STD */
#include <vector>

/* OpenColorIO */
#include <OpenColorIO/OpenColorIO.h>

/* Internal */
#include "Definition.h"
#include "Colorspace.h"
#include "Image.h"

VOID_NAMESPACE_OPEN

class VOID_API ColorProcessor
{
    ColorProcessor();

public: /* Enum */
    enum class Config
    {
        Builtin,
        Environment
    };

public:
    static ColorProcessor& Instance();

    void SetConfig(const Config& type = Config::Builtin);
    void SetConfig(const std::string& path);
    void SetConfig(std::istream& stream);

    /**
     * Value Getters
     */
    std::vector<std::string> Displays() const;
    std::vector<std::string> Views(const std::string& display) const;
    std::vector<std::string> Colorspaces() const;

    /**
     * Default display value from the current config
     */
    inline std::string DefaultDisplay() const { return m_Config->getDefaultDisplay(); }
    inline std::string DefaultView(const std::string& display) const { return m_Config->getDefaultView(display.c_str()); }

    /**
     * Setup Processor Config
     */
    void Create();
    void Create(const std::string& display);
    void Create(const std::string& source, const std::string& destination);
    void Create(const std::string& inputcolorspace, const std::string& display, const std::string& view);

    /**
     * Set Display for the processor
     */
    void Set(const std::string& display);
    void Set(const std::string& display, const std::string& view);

    void ResetDisplay();

    /**
     * Returns the Generated shader code from the Constructed GPU Processor
     */
    std::string Shader(const std::string& function) const;

    // CPU processing for the image from the source to destination colorspace
    void ProcessImage(float* pixels, int width, int height, int channels, const std::string& outcolorspace) const;
    // void ProcessImage(float* pixels, int width, int height, int channels, const ColorSpace& colorspace, const std::string& outcolorspace) const;

private:
    OCIO_NAMESPACE::ConstConfigRcPtr m_Config;
    OCIO_NAMESPACE::ConstGPUProcessorRcPtr m_GProcessor;
};

/// @brief Processes color for media to normalize the colorspace of the media in VOID
/// Internally our working space is ACEScg based on it's wider range of color gamut
/// This processor ensures that all the media we're dealing with, switches to that working space
/// later other transforms can be applied before finally rendering out
class VOID_API MediaColorProcessor
{
    struct ProcessorEntry
    {
        // Colorspace colorspace;
        MediaColor color;
        OCIO_NAMESPACE::ConstProcessorRcPtr processor = nullptr;
        ProcessorEntry() = default;
        ProcessorEntry(const MediaColor& color, const OCIO_NAMESPACE::ConstConfigRcPtr& config);
    };

    MediaColorProcessor();
public:
    static MediaColorProcessor& Instance();
    std::vector<std::string> Colorspaces() const;
    void Process(const MediaColor& color, const FloatImage& image);

private:
    // std::unordered_map
    std::vector<MediaColorProcessor::ProcessorEntry> m_Processors;
    OCIO_NAMESPACE::ConstConfigRcPtr m_Config;
    MediaColorProcessor::ProcessorEntry* m_Processor;
    // OCIO_NAMESPACE::ConstCPUProcessorRcPtr m_Processor;

private:
    OCIO_NAMESPACE::ConstProcessorRcPtr Processor(const MediaColor& color);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_COLOR_PROCESSOR_H
