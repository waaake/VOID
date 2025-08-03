// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Qt */
#include <QByteArray>
#include <QFile>

/* Internal */
#include "ColorController.h"
#include "VoidCore/ColorProcessor.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

ColorController::ColorController(QWidget* parent)
    : ControlCombo(parent)
{
    InitColorProcessor();

    Setup();

    /* Color Display */
    connect(this, &QComboBox::currentTextChanged, this, [this](const QString& value) { emit colorDisplayChanged(value.toStdString());});
}

ColorController::~ColorController()
{
}

void ColorController::InitColorProcessor()
{
    VOID_LOG_INFO("Initializing Color Processor");

    /* Color Processor Setup */
    ColorProcessor& proc = ColorProcessor::Instance();

    /* Setup the config */
    /**
     * Read the Environment Variable OCIO first,
     * if that exists, gets precedence
     */
    const char* env_ = std::getenv("OCIO");

    if (env_)
    {
        VOID_LOG_INFO("Using Color Config from Environment.");
        proc.SetConfig(ColorProcessor::Config::Environment);
    }
    else
    {
        /* Read the File as Byte Array */
        QFile ff(":resources/config/void-default.ocio");

        /* Cannot read the File */
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

    /* Setup default color display */
    proc.SetDefaultDisplay();
}

void ColorController::Setup()
{
    /* Color Display Control */
    ColorProcessor& proc = ColorProcessor::Instance();

    for (std::string& display : proc.Displays())
    {
        addItem(display.c_str());
    }

    /* Default */
    setCurrentText(proc.DefaultDisplay().c_str());
}

VOID_NAMESPACE_CLOSE
