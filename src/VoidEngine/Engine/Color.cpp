// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Qt */
#include <QByteArray>
#include <QFile>

/* Internal */
#include "Color.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Processors/ColorProcessor.h"

VOID_NAMESPACE_OPEN

void ColorConfig::Init()
{
    VOID_LOG_INFO("Initializing Color Config");
    ColorProcessor& proc = ColorProcessor::Instance();

    /// Config Setup
    /// Order
    /// 1. Environment, if available
    /// 2. Internal VOID OCIO config
    /// 3. If that fails, we use Builtin OCIO config
    const char* env_ = std::getenv("OCIO");
    if (env_)
    {
        VOID_LOG_INFO("Using Color Config from Environment.");
        proc.SetConfig(ColorProcessor::Config::Environment);
    }
    else
    {
        // As byte array
        QFile ff(":resources/config/void-default.ocio");
        if (!ff.open(QIODevice::ReadOnly))
        {
            VOID_LOG_INFO("Using Builtin Color Config.");
            proc.SetConfig(ColorProcessor::Config::Builtin);
        }
        else
        {
            VOID_LOG_INFO("Using Void Default Color Config.");
            QByteArray data = ff.readAll();
            std::istringstream stream(data.toStdString());
            proc.SetConfig(stream);
        }
    }

    proc.ResetDisplay();
}

VOID_NAMESPACE_CLOSE
