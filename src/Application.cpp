/* Qt */
#include <QApplication>

/* Internal */
#include "Definition.h"
#include "VoidCore/Logging.h"
#include "VoidUi/PlayerWindow.h"
#include "VoidUi/VoidStyle.h"

int main(int argc, char* argv[])
{
    #ifdef VOID_ENABLE_LOGGING
    /* Initialise the Logger */
    VOID_NAMESPACE::Logger::Init();
    #endif

    QApplication app(argc, argv);

    VOID_LOG_INFO("App initialised.");

    VOID_NAMESPACE::VoidMainWindow* imager = new voidplayer::VoidMainWindow;

    VOID_LOG_INFO("Imager initialised.");

    /* Set Application icon */
    QImage icon(":resources/images/VOID_Logo.svg");

    app.setWindowIcon(QIcon(QPixmap::fromImage(icon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation))));

    app.setStyle(new VOID_NAMESPACE::VoidDark());

    /* Show the player */
    imager->show();

    app.exec();

    VOID_LOG_INFO("App Terminated.");

    /* Delete from the heap */
    delete imager;
    return 0;
}
