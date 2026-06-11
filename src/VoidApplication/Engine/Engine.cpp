// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstdlib>
#include <iostream>

/* Internal */
#include "Engine.h"
/* LOGGING */
#include "VoidCore/Logging.h"
/* Theme */
#include "VoidStyle.h"
/* Plugin Registration */
#include "VoidCore/Plugins/Loader.h"
#include "VoidCore/Readers/Registration.h"
#include "VoidCore/Operators/Registration.h"
#include "VoidCore/Writers/Registration.h"

#include "VoidCore/Profiler.h"
#include "VoidObjects/Core/Threads.h"
#include "VoidUi/Engine/Bridge.h"
#include "VoidUi/Engine/Globals.h"
#include "VoidUi/Preferences/Preferences.h"
#include "VoidUi/BaseWindow/StartupWindow.h"

VOID_NAMESPACE_OPEN

VoidEngine::~VoidEngine()
{
    VOID_LOG_INFO("App Terminated.");
}

int VoidEngine::Exec(int argc, char** argv)
{
    /* Parse incoming args to get what can be used during/post startup */
    m_Args = ArgParser::ParseArgs(argc, argv);

    InitLogging();

    if (m_Args.help)
        return PrintHelp();

    if (m_Args.scale > 1)
    {
        #if defined(_VOID_PLATFORM_WINDOWS)
        _putenv_s("QT_SCALE_FACTOR", std::to_string(m_Args.scale).c_str());
        #else
        setenv("QT_SCALE_FACTOR", std::to_string(m_Args.scale).c_str(), 1);
        #endif
    }

    #if _QT6
    #else
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #endif

    QApplication app(argc, argv);

    Setup(app);
    Initialize();

    /* Load Plugins etc. */
    PostInit();

    m_Imager->showMaximized();

    /* Color Theme change based on the preference */
    m_Imager->connect(&VoidPreferences::Instance(), &VoidPreferences::updated, m_Imager, [&]()
    {
        app.setStyle(VoidColorStyle::GetProxyStyle(
            static_cast<VoidColorStyle::StyleType>(VoidPreferences::Instance().GetColorStyle())
        ));
        UIGlobals::SetLuminance(app.palette().color(QPalette::Window));
    });

    /* Once the UI is up -> Process any further events or windows */
    PostStartup();

    int result = app.exec();

    /* Cleanup */
    ThreadPool::Destroy();
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
    UIGlobals::SetLuminance(app.palette().color(QPalette::Window));

    /* Set Application icon */
    QImage icon(":resources/images/VOID_Logo.svg");
    app.setWindowIcon(QIcon(QPixmap::fromImage(icon.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation))));

    if (m_Args.framerate > 0)
        UIGlobals::SetFramerate(m_Args.framerate);

    UIGlobals::ToggleAudio(m_Args.audio);
}

void VoidEngine::Initialize()
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("VoidEngine::Initialize");
    ThreadPool::Init();
    m_Imager = new VoidMainWindow;
    UIGlobals::g_VoidMainWindow = m_Imager;

    // Workspace
    m_Imager->SwitchWorkspace(m_Args.basic
                ? Workspace::BASIC
                : static_cast<Workspace>(VoidPreferences::Instance().GetDefaultWorkspace()));

    /* Init Menu */
    m_MenuSystem = new MenuSystem(m_Imager);
    UIGlobals::g_MenuSystem = m_MenuSystem;
    m_Imager->InitMenu(m_MenuSystem);

    VOID_LOG_INFO("Imager Initialized.");
}

void VoidEngine::PostInit()
{
    // Register Media Readers
    RegisterReaders();
    // Register Operators
    RegisterOperators();
    // Register Media Writers
    RegisterWriters();

    /* Register Any other Media plugins that are found in the way */
    // ReaderPluginLoader::Instance().LoadExternals();
}

void VoidEngine::PostStartup()
{
    if (!m_Args.project.empty())
        EngineBridge::OpenProject(m_Args.project);
    else if (!m_Args.media.empty())
        EngineBridge::LoadMedia(m_Args.media);

    /* Defer the callbacks */
    QTimer::singleShot(800, m_Imager, [this]() -> void { Callback(); });
}

void VoidEngine::Callback()
{
    if (m_Args.project.empty() && m_Args.media.empty() && !m_Args.basic)
    {
        /* Startup window pop-up */
        StartupWindow::Exec(m_Imager);
    }
}

int VoidEngine::PrintHelp()
{
    #if defined(_VOID_PLATFORM_WINDOWS)
    std::cout << "Usage: VOID.exe [options]" << "\n";
    #else
    std::cout << "Usage: VOID [options]" << "\n";
    #endif

    // Options
    std::cout << "\n" << "Options:" << "\n";

    // Help is separate
    std::cout << "  --help              Shows this help message" << "\n\n";

    // Main args
    std::cout << "  --audio             Enable audio playback" << "\n";
    std::cout << "  --basic             Launch VOID with basic workspace" << "\n";
    std::cout << "  --framerate         Specify the default framerate for VOID" << "\n";
    std::cout << "  --media             Specify media to open/load at startup" << "\n";
    std::cout << "  --project           Specify project to load at startup (.ether .nether)" << "\n";
    std::cout << "  --scale             Specify the scale factor for the UI, defaults to using the DPI scaling from the system" << "\n";

    // Examples
    std::cout << "\n" << "Examples:" << "\n";
    std::cout << "  VOID --project /path/to/project/file.ether" << "\n";
    std::cout << "  VOID --media /path/to/media.mp4" << "\n";
    std::cout << "  VOID --media /path/to/image/sequence.####.png" << "\n";
    std::cout << "  VOID --media /path/to/another/media.mov --basic" << "\n";
    std::cout << "  VOID --framerate 23.976" << "\n";
    std::cout << "  VOID --framerate 30.0" << "\n";
    std::cout << "  VOID --audio" << "\n";
    std::cout << "  VOID --scale 2" << "\n";

    // Final block
    std::cout << "\n";

    return 0;
}

VOID_NAMESPACE_CLOSE
