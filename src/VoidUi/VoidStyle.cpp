/* Qt */
#include <QStyleOption>

/* Internal */
#include "VoidStyle.h"

VOID_NAMESPACE_OPEN

VoidDark::VoidDark()
    : QProxyStyle("Fusion")
{
    
}

void VoidDark::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    // if (element == PE_PanelButtonCommand)
    // {
    //     painter->fillRect(option->rect, QColor(53, 53, 53));
        
    //     return;
    // }

    switch (element)
    {
        case PE_PanelButtonCommand:
            /* Dark buttons */
            painter->fillRect(option->rect, QColor(53, 53, 53));
            break;
        // case PE_FrameWindow:
        // case PE_Widget:
        //     /* Dark window background */
        //     painter->fillRect(option->rect, QColor(40, 40, 40));
        //     break;
        default:
            /* Let everything else get drawn */
            QProxyStyle::drawPrimitive(element, option, painter, widget);
    }

    // /* Let everything else get drawn */
    // QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void VoidDark::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    switch (element)
    {
        // case CE_ShapedFrame:
        //     /* Dark frame background */
        //     painter->fillRect(option->rect, QColor(40, 40, 40));
        //     break;
        // case CE_MenuBarEmptyArea:
        //     painter->fillRect(option->rect, QColor(40, 40, 40));
        //     break;
        default:
            QProxyStyle::drawControl(element, option, painter, widget);
    }
}

void VoidDark::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(40, 40, 40));
    palette.setColor(QPalette::WindowText, QColor(210, 210, 210));
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, QColor(250, 250, 200));
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, QColor(210, 210, 210));
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, QColor(210, 210, 210));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, VOID_HIGHLIGHT_GREEN);
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Dark, QColor(20, 20, 20));
}

VOID_NAMESPACE_CLOSE
