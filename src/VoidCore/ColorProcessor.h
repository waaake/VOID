#ifndef _VOID_COLOR_PROCESSOR_H
#define _VOID_COLOR_PROCESSOR_H

/* STD */
#include <vector>

/* OpenColorIO */
#include <OpenColorIO/OpenColorIO.h>

/* Internal */
#include "Definition.h"

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

    /* Singleton Instance of the Processor for the Rendering System */
    static ColorProcessor& Instance()
    {
        static ColorProcessor processor;
        return processor;
    }

    void SetConfig(const Config& type = Config::Builtin);
    void SetConfig(const std::string& path);

    /**
     * Value Getters
     */
    std::vector<std::string> Displays() const;
    std::vector<std::string> Views(const std::string& display) const;
    std::vector<std::string> Colorspaces() const;

    /**
     * Setup Processor Config
     */
    void Create();
    void Create(const std::string& source, const std::string& destination);
    void Create(const std::string& inputcolorspace, const std::string& display, const std::string& view);

    /**
     * Returns the Generated shader code from the Constructed GPU Processor
     */
    std::string Shader(const std::string& function) const;

private:
    OCIO_NAMESPACE::ConstConfigRcPtr m_Config;
    OCIO_NAMESPACE::ConstGPUProcessorRcPtr m_GProcessor;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_COLOR_PROCESSOR_H
