// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QApplication>

/* Internal */
#include "Definition.h"
/* LOGGING */
#include "VoidCore/Logging.h"
/* Main Window for player */
#include "VoidUi/VoidGlobals.h"
#include "VoidUi/BaseWindow/PlayerWindow.h"
/* Theme */
#include "VoidUi/VoidStyle.h"
/* Reader Registration */
#include "VoidCore/Plugins/Loader.h"
#include "VoidCore/Readers/Registration.h"


int main(int argc, char* argv[])
{
    #ifdef VOID_ENABLE_LOGGING
    /* Initialise the Logger */
    VOID_NAMESPACE::Logger::Init();
    #endif

    QApplication app(argc, argv);
    app.setStyle(new VOID_NAMESPACE::VoidDark());

    VOID_LOG_INFO("App Initialized.");

    /* App Settings */
    QCoreApplication::setOrganizationName("VOID");
    QCoreApplication::setApplicationName("VOID");

    VOID_NAMESPACE::VoidMainWindow* imager = new voidplayer::VoidMainWindow;
    VOID_NAMESPACE::g_VoidMainWindow = imager;

    VOID_LOG_INFO("Imager Initialized.");

    /* Register Media Readers */
    VOID_NAMESPACE::RegisterReaders();

    /* Register Any other Media plugins that are found in the way */
    // VOID_NAMESPACE::ReaderPluginLoader::Instance().LoadExternals();

    /* Set Application icon */
    QImage icon(":resources/images/VOID_Logo.svg");

    app.setWindowIcon(QIcon(QPixmap::fromImage(icon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation))));

    /* Show the player */
    imager->show();

    app.exec();

    VOID_LOG_INFO("App Terminated.");

    /* Delete from the heap */
    delete imager;
    return 0;
}
