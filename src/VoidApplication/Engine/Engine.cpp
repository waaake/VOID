// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Engine.h"
/* LOGGING */
#include "VoidCore/Logging.h"
/* Theme */
#include "VoidStyle.h"
/* Reader Registration */
#include "VoidCore/Plugins/Loader.h"
#include "VoidCore/Readers/Registration.h"
#include "VoidUi/Engine/Globals.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

VoidEngine::~VoidEngine()
{
    VOID_LOG_INFO("App Terminated.");
}

int VoidEngine::Exec(int argc, char** argv)
{
    InitLogging();

    QApplication app(argc, argv);

    Setup(app);
    Initialize();

    /* Load Plugins etc. */
    PostInit();

    m_Imager->showMaximized();
    int result = app.exec();

    /* Cleanup */
    delete m_MenuSystem;
    delete m_Imager;
    return result;
}

void VoidEngine::InitLogging()
{
    #ifdef VOID_ENABLE_LOGGING
    /* Initialise the Logger */
    Logger::Init();
    #endif
}

void VoidEngine::Setup(QApplication& app)
{
    /* App Settings */
    QCoreApplication::setOrganizationName("VOID");
    QCoreApplication::setApplicationName("VOID");

    /* Setup Theme for the Player */
    app.setStyle(VoidColorStyle::GetProxyStyle(
        static_cast<VoidColorStyle::StyleType>(VoidPreferences::Instance().GetColorStyle())
    ));

    /* Set Application icon */
    QImage icon(":resources/images/VOID_Logo.svg");
    app.setWindowIcon(QIcon(QPixmap::fromImage(icon.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation))));
}

void VoidEngine::Initialize()
{
    m_Imager = new VoidMainWindow;
    UIGlobals::g_VoidMainWindow = m_Imager;

    /* Init Menu */
    m_MenuSystem = new MenuSystem(m_Imager);
    UIGlobals::g_MenuSystem = m_MenuSystem;
    m_Imager->InitMenu(m_MenuSystem);

    VOID_LOG_INFO("Imager Initialized.");
}

void VoidEngine::PostInit()
{
    /* Register Media Readers */
    RegisterReaders();

    /* Register Any other Media plugins that are found in the way */
    // ReaderPluginLoader::Instance().LoadExternals();
}


VOID_NAMESPACE_CLOSE
