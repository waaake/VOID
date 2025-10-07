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
        case CE_MenuBarItem:
        {
            const QStyleOptionMenuItem* item = qstyleoption_cast<const QStyleOptionMenuItem*>(option);

            if (!item)
                break;
   
            if (item->state & QStyle::State_Sunken)
            {
                painter->save();
                painter->fillRect(item->rect, option->palette.color(QPalette::Highlight));
                
                painter->setPen(option->palette.color(QPalette::Highlight).lighter(80));
                painter->drawRect(item->rect.adjusted(0, 0, -1, -1));
                painter->restore();
            }

            painter->save();
            painter->setPen(QPen(item->state & QStyle::State_Sunken
                ? option->palette.color(QPalette::HighlightedText)
                : option->palette.color(QPalette::WindowText)));
            painter->drawText(item->rect, Qt::AlignCenter, item->text);
            painter->restore();
            break;
        }
        case CE_MenuBarEmptyArea:
        {
            painter->save();
            painter->fillRect(option->rect, option->palette.window());
            painter->restore();
            break;
        }
        case CE_TabBarTab:
        {
            const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(option);

            if (!tab)
                break;

            painter->save();

            /* Background */
            painter->fillRect(tab->rect, option->palette.color(QPalette::Window));
            QColor tabColor = tab->state & State_Selected ? option->palette.color(QPalette::Window) : option->palette.color(QPalette::Window).darker(125);
            painter->fillRect(tab->rect.adjusted(0, 0, 0, -2), tabColor);

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
    palette.setColor(QPalette::AlternateBase, QColor(43, 43, 43));
    palette.setColor(QPalette::ToolTipBase, QColor(250, 250, 200));
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, VOID_FOREGROUND_COLOR);
    palette.setColor(QPalette::Button, QColor(48, 48, 48));
    palette.setColor(QPalette::ButtonText, VOID_FOREGROUND_COLOR);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, VOID_HIGHLIGHT_GREEN);
    palette.setColor(QPalette::HighlightedText, Qt::black);
    // palette.setColor(QPalette::Mid, QColor(40, 40, 40));
    palette.setColor(QPalette::Dark, QColor(25, 25, 25));

    palette.setColor(QPalette::Disabled, QPalette::Text, VOID_FOREGROUND_DISABLED_COLOR);
}

/* Blue Shore {{{ */

BlueShore::BlueShore()
    : VoidDark()
{
}

void BlueShore::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(122, 174, 209));
    palette.setColor(QPalette::WindowText, QColor(47, 62, 78));
    palette.setColor(QPalette::Base, QColor(227, 240, 250));
    palette.setColor(QPalette::AlternateBase, QColor(216, 234, 247));
    palette.setColor(QPalette::ToolTipBase, QColor(240, 248, 255));
    palette.setColor(QPalette::ToolTipText, QColor(60, 79, 92));
    palette.setColor(QPalette::Text, QColor(42, 58, 74));
    palette.setColor(QPalette::Button, QColor(180, 205, 230));
    palette.setColor(QPalette::ButtonText, QColor(47, 62, 78));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Link, QColor(74, 144, 226));
    palette.setColor(QPalette::Highlight, QColor(108, 160, 220));
    palette.setColor(QPalette::HighlightedText, QColor(248, 252, 255));
    palette.setColor(QPalette::Dark, QColor(90, 107, 122));

    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(138, 155, 170));
}

/* }}} */

/* Sakura Pink {{{ */

SakuraPink::SakuraPink()
    : VoidDark()
{
}

void SakuraPink::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(239, 202, 214));
    palette.setColor(QPalette::WindowText, QColor(75, 44, 58));
    palette.setColor(QPalette::Base, QColor(246, 217, 221));
    palette.setColor(QPalette::AlternateBase, QColor(250, 218, 221));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 240, 245));
    palette.setColor(QPalette::ToolTipText, QColor(92, 75, 81));
    palette.setColor(QPalette::Text, QColor(90, 58, 69));
    palette.setColor(QPalette::Button, QColor(244, 194, 194));
    palette.setColor(QPalette::ButtonText, QColor(75, 59, 71));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Link, QColor(201, 124, 158));
    palette.setColor(QPalette::Highlight, QColor(212, 138, 156));
    palette.setColor(QPalette::HighlightedText, QColor(75, 44, 58));
    palette.setColor(QPalette::Dark, QColor(107, 86, 93));

    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(160, 138, 149));
}

/* }}} */

/* Obsidian {{{ */

Obsidian::Obsidian()
    : VoidDark()
{
}

void Obsidian::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(30, 30, 30));
    palette.setColor(QPalette::WindowText, QColor(208, 208, 208));
    palette.setColor(QPalette::Base, QColor(42, 42, 42));
    palette.setColor(QPalette::AlternateBase, QColor(51, 51, 51));
    palette.setColor(QPalette::ToolTipBase, QColor(46, 46, 46));
    palette.setColor(QPalette::ToolTipText, QColor(192, 192, 192));
    palette.setColor(QPalette::Text, QColor(185, 185, 185));
    palette.setColor(QPalette::Button, QColor(47, 47, 47));
    palette.setColor(QPalette::ButtonText, QColor(208, 208, 208));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Link, QColor(102, 255, 153));
    palette.setColor(QPalette::Highlight, QColor(160, 190, 60));
    palette.setColor(QPalette::HighlightedText, QColor(26, 26, 26));
    palette.setColor(QPalette::Dark, QColor(42, 42, 42));

    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(128, 128, 128));
}

/* }}} */

VoidDark* VoidColorStyle::GetProxyStyle(const VoidColorStyle::StyleType& type)
{
    switch (type)
    {
        case VoidColorStyle::StyleType::Obsidian:
            return new Obsidian;
        case VoidColorStyle::StyleType::SakuraPink:
            return new SakuraPink;
        case VoidColorStyle::StyleType::ShoreBlue:
            return new BlueShore;
        default:
            return new VoidDark;
    }
}

VOID_NAMESPACE_CLOSE
