// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QStyleOption>

/* Internal */
#include "VoidStyle.h"

VOID_NAMESPACE_OPEN

VoidDark::VoidDark()
    : QProxyStyle("Fusion")
{
}

// void VoidDark::drawItemText(QPainter* painter, const QRect& rect, int flags, const QPalette& palette, bool enabled, const QString& text, QPalette::ColorRole textRole) const
// {
//     VOID_LOG_INFO(text.toStdString());
//     if (!enabled)
//     {
//         QColor disabled = QColor(240, 240, 240); // palette.color(QPalette::Disabled, textRole);
//         painter->setPen(disabled);
//         painter->drawText(rect, flags, text);   // Draw text with disabled text color
//     }
//     else
//     {
//         /* Base drawing */
//         QProxyStyle::drawItemText(painter, rect, flags, palette, enabled, text, textRole);
//     }
// }

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
            if (option->state & State_Sunken)
                painter->fillRect(option->rect, option->palette.color(QPalette::Button).darker(150));
            else if (option->state & State_MouseOver)
                painter->fillRect(option->rect, option->palette.color(QPalette::Button).lighter(150));
            else
                painter->fillRect(option->rect, option->palette.color(QPalette::Button));
            break;
        case PE_FrameTabWidget:
            painter->save();
            painter->setBrush(option->palette.color(QPalette::Window));
            painter->setPen(Qt::NoPen);
            painter->drawRect(option->rect);
            painter->restore();
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
        case CE_MenuItem:
        {
            const QStyleOptionMenuItem* item = qstyleoption_cast<const QStyleOptionMenuItem*>(option);

            if (!item)
                break;

            /* Get the sub element rect for the icon */
            QRect textRect = item->rect;
            /* Consider the icon width */
            textRect.setLeft(textRect.left() + item->maxIconWidth + 6); // Add the spacing across icon sides

            if (item->state & QStyle::State_Enabled)
            {
                /* Default */
                QProxyStyle::drawControl(element, option, painter, widget);
                break;
            }

            painter->setPen(VOID_FOREGROUND_DISABLED_COLOR);
            painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, item->text);
            break;

        }
        case CE_TabBarTab:
        {
            const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(option);

            if (!tab)
                break;

            QRect rect = tab->rect;
            painter->save();

            /* Background */
            QColor tabColor = tab->state & State_Selected ? option->palette.color(QPalette::Window) : option->palette.color(QPalette::Window).darker(200);
            painter->fillRect(rect, tabColor);

            /* Text */
            QRect textRect = subElementRect(SE_TabBarTabText, tab, widget);
            QColor textColor = tab->state & State_Selected ? option->palette.color(QPalette::WindowText) : option->palette.color(QPalette::WindowText).darker(150);
            painter->setPen(textColor);
            painter->drawText(textRect, Qt::AlignCenter, tab->text);

            // /* Outer Border */
            // painter->setPen(QPen(Qt::black, 1));
            // painter->drawRect(option->rect.adjusted(0, 0, -1, -1));

            painter->restore();
            break;
        }
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
    palette.setColor(QPalette::WindowText, VOID_FOREGROUND_COLOR);
    palette.setColor(QPalette::Base, QColor(30, 30, 30));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, QColor(250, 250, 200));
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, VOID_FOREGROUND_COLOR);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, VOID_FOREGROUND_COLOR);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, VOID_HIGHLIGHT_GREEN);
    palette.setColor(QPalette::HighlightedText, Qt::black);
    // palette.setColor(QPalette::Mid, QColor(40, 40, 40));
    palette.setColor(QPalette::Dark, QColor(25, 25, 25));

    palette.setColor(QPalette::Disabled, QPalette::Text, VOID_FOREGROUND_DISABLED_COLOR);
}

/* Void Blue {{{ */

VoidBlue::VoidBlue()
    : VoidDark()
{
}

void VoidBlue::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(50, 120, 170));
    palette.setColor(QPalette::WindowText, QColor(30, 30, 30));
    palette.setColor(QPalette::Base, QColor(10, 100, 150));
    palette.setColor(QPalette::AlternateBase, QColor(30, 90, 140));
    palette.setColor(QPalette::ToolTipBase, QColor(250, 250, 200));
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, QColor(30, 30, 30));
    palette.setColor(QPalette::Button, QColor(30, 90, 140));
    palette.setColor(QPalette::ButtonText, QColor(30, 30, 30));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(32, 230, 218));
    palette.setColor(QPalette::Highlight, QColor(200, 70, 150));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Dark, QColor(10, 80, 120));

    palette.setColor(QPalette::Disabled, QPalette::Text, VOID_FOREGROUND_DISABLED_COLOR);
}

/* }}} */

VOID_NAMESPACE_CLOSE
