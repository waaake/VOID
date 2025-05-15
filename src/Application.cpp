/* Qt */
#include <QApplication>
// #include <QStyleFactory>

/* Internal */
#include "Definition.h"
#include "VoidUi/PlayerWindow.h"
#include "VoidUi/VoidStyle.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    VOID_NAMESPACE::VoidMainWindow* imager = new voidplayer::VoidMainWindow;

    /**
    VOID_NAMESPACE::VoidImageSequence m_Sequence;
    m_Sequence.Read("/mnt/c/Skids/zework/basic");
    
    VOID_NAMESPACE::VoidMediaItem* imager = new VOID_NAMESPACE::VoidMediaItem(m_Sequence);
    */

    // Darker theme palette
    QPalette palette;

    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);

    /* Apply darker palette */
    // app.setPalette(palette);

    /* Set Application icon */
    QImage icon("images/VOID_Logo.svg");

    app.setWindowIcon(QIcon(QPixmap::fromImage(icon.scaled(48, 48, Qt::KeepAspectRatio))));

    /* Ensure background is filled */
    // imager->setAutoFillBackground(true);

    // app.setStyle(QStyleFactory::create("Fusion"));
    app.setStyle(new VOID_NAMESPACE::VoidDark());

    /* Show the player */
    imager->show();

    app.exec();

    /* Delete from the heap */
    delete imager;
    return 0;
}