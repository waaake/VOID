// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Qt */
#include <QByteArray>
#include <QFile>

/* Internal */
#include "ColorEngine.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

namespace OCIO = OCIO_NAMESPACE;

ColorEngine::ColorEngine(QObject* parent)
    : QObject(parent)
    , m_Processor(ColorProcessor::Instance())
{
    Init();
}

void ColorEngine::Init()
{
    VOID_LOG_INFO("Initializing Color Config");

    /// Config Setup
    /// Order
    /// 1. Environment, if available
    /// 2. Internal VOID OCIO config
    /// 3. If that fails, we use Builtin OCIO config
    const char* env_ = std::getenv("OCIO");
    if (env_)
    {
        VOID_LOG_INFO("Using Color Config from Environment.");
        m_Processor.SetConfig(ColorProcessor::Config::Environment);
    }
    else
    {
        // As byte array
        QFile ff(":resources/config/void-default.ocio");
        if (!ff.open(QIODevice::ReadOnly))
        {
            VOID_LOG_INFO("Using Builtin Color Config.");
            m_Processor.SetConfig(ColorProcessor::Config::Builtin);
        }
        else
        {
            VOID_LOG_INFO("Using Void Default Color Config.");
            QByteArray data = ff.readAll();
            std::istringstream stream(data.toStdString());
            m_Processor.SetConfig(stream);
        }
    }

    m_Processor.ResetDisplay();

    /// Setup the defaults
    const std::string& display = m_Processor.DefaultDisplay();
    m_View = m_Processor.DefaultView(display).c_str();
    m_Display = display.c_str();
}

QStringList ColorEngine::Roles() const
{
    QStringList r;
    const OCIO::ConstConfigRcPtr& config = m_Processor.OCIOConfig();

    const int num = config->getNumRoles();
    r.reserve(num);

    for (int i = 0; i < num; ++i)
        r.append(config->getRoleName(i));

    return r;
}

QStringList ColorEngine::Views() const
{
    QStringList v;
    const OCIO::ConstConfigRcPtr& config = m_Processor.OCIOConfig();
    
    const std::string& display = m_Display.toStdString();
    const int num = config->getNumViews(display.c_str());
    v.reserve(num);

    for (int i = 0; i < num; ++i)
        v.append(config->getView(display.c_str(), i));

    return v;
}

QStringList ColorEngine::Displays() const
{
    QStringList d;
    const OCIO::ConstConfigRcPtr& config = m_Processor.OCIOConfig();

    const int num = config->getNumDisplays();
    d.reserve(num);

    for (int i = 0; i < num; ++i)
        d.append(config->getDisplay(i));

    return d;
}

QString ColorEngine::WorkingSpace() const
{
    return m_Processor.WorkingSpace().c_str();
}

void ColorEngine::SetDisplay(const QString& display)
{
    m_Display = display;
    const std::string& d = display.toStdString();
    m_View = m_Processor.DefaultView(d).c_str();
    m_Processor.Set(d, m_View.toStdString());

    emit displayChanged();
    emit updated();
}

void ColorEngine::SetView(const QString& view)
{
    m_View = view;
    // const std::string& d = m_Display.toStdString();
    // const std::string& v = view.toStdString();
    m_Processor.Set(m_Display.toStdString(), view.toStdString());

    emit viewChanged();
    emit updated();
}

VOID_NAMESPACE_CLOSE
