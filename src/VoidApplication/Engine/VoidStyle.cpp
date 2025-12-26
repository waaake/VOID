// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QStyleOption>

/* Internal */
#include "VoidStyle.h"

VOID_NAMESPACE_OPEN

VoidBase::VoidBase()
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

void VoidBase::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
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
                painter->fillRect(option->rect, option->palette.color(QPalette::AlternateBase));
            else if (option->state & State_MouseOver)
                painter->fillRect(option->rect, option->palette.color(QPalette::Base));
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

void VoidBase::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
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

            painter->setPen(option->palette.color(QPalette::Disabled, QPalette::Text));
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

/* Void White {{{ */

VoidWhite::VoidWhite()
{
}

/* }}} */

/* Void Dark {{{ */

VoidDark::VoidDark()
    : VoidBase()
{
}

void VoidDark::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(40, 40, 40));
    palette.setColor(QPalette::WindowText, QColor(190, 190, 190));
    palette.setColor(QPalette::Base, QColor(43, 43, 43));
    palette.setColor(QPalette::AlternateBase, QColor(30, 30, 30));
    palette.setColor(QPalette::ToolTipBase, QColor(250, 250, 200));
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, QColor(190, 190, 190));
    palette.setColor(QPalette::Button, QColor(48, 48, 48));
    palette.setColor(QPalette::ButtonText, QColor(190, 190, 190));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(160, 190, 60));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    // palette.setColor(QPalette::Mid, QColor(40, 40, 40));
    palette.setColor(QPalette::Dark, QColor(25, 25, 25));

    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(80, 80, 80));
}

/* }}} */

/* Night Blue {{{ */

NightBlue::NightBlue()
    : VoidBase()
{
}

void NightBlue::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(45, 55, 75));
    palette.setColor(QPalette::Base, QColor(55, 75, 95));
    palette.setColor(QPalette::AlternateBase, QColor(45, 65, 85));
    palette.setColor(QPalette::Dark, QColor(50, 70, 90));
    palette.setColor(QPalette::WindowText, QColor(240, 245, 255));
    palette.setColor(QPalette::Text, QColor(225, 235, 250));
    palette.setColor(QPalette::ButtonText, QColor(240, 250, 255));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(230, 240, 255));
    palette.setColor(QPalette::Button, QColor(70, 90, 120));
    palette.setColor(QPalette::ToolTipBase, QColor(75, 95, 125));
    palette.setColor(QPalette::Link, QColor(0, 150, 255));
    palette.setColor(QPalette::Highlight, QColor(105, 140, 255));
    palette.setColor(QPalette::HighlightedText, QColor(20, 30, 45));
    palette.setColor(QPalette::Light, QColor(255, 200, 80));
    palette.setColor(QPalette::Midlight, QColor(255, 160, 60));

    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(150, 160, 175));
}

/* }}} */

/* Sakura Pink {{{ */

SakuraPink::SakuraPink()
    : VoidBase()
{
}

void SakuraPink::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(248, 235, 240));
    palette.setColor(QPalette::Base, QColor(249, 243, 234));
    palette.setColor(QPalette::AlternateBase, QColor(250, 230, 240));
    palette.setColor(QPalette::Dark, QColor(220, 190, 200));
    palette.setColor(QPalette::WindowText, QColor(50, 30, 40));
    palette.setColor(QPalette::Text, QColor(60, 40, 50));
    palette.setColor(QPalette::ButtonText, QColor(70, 50, 60));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(60, 40, 50));
    palette.setColor(QPalette::Button, QColor(232, 185, 200));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 240, 245));
    palette.setColor(QPalette::Link, QColor(200, 80, 120));
    palette.setColor(QPalette::Highlight, QColor(234, 172, 200));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    palette.setColor(QPalette::Light, QColor(255, 200, 220));
    palette.setColor(QPalette::Midlight, QColor(255, 170, 190));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(160, 140, 150));
}

/* }}} */

/* Obsidian {{{ */

Obsidian::Obsidian()
    : VoidBase()
{
}

void Obsidian::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(30, 30, 30));
    palette.setColor(QPalette::WindowText, QColor(208, 208, 208));
    palette.setColor(QPalette::Base, QColor(51, 51, 51));
    palette.setColor(QPalette::AlternateBase, QColor(42, 42, 42));
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

/* Void Gray {{{ */

VoidGray::VoidGray()
    : VoidBase()
{
}

void VoidGray::polish(QPalette& palette)
{
    palette.setColor(QPalette::Window, QColor(57, 57, 57));
    palette.setColor(QPalette::Base, QColor(66, 66, 66));
    palette.setColor(QPalette::AlternateBase, QColor(51, 51, 51));
    palette.setColor(QPalette::Dark, QColor(35, 35, 35));
    palette.setColor(QPalette::WindowText, QColor(190, 190, 190));
    palette.setColor(QPalette::Text, QColor(220, 220, 220));
    palette.setColor(QPalette::ButtonText, QColor(210, 210, 210));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(230, 230, 230));
    palette.setColor(QPalette::Button, QColor(91, 91, 91));
    palette.setColor(QPalette::ToolTipBase, QColor(60, 60, 60));
    palette.setColor(QPalette::Link, QColor(90, 150, 250));
    palette.setColor(QPalette::Highlight, QColor(255, 152, 20));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Light, QColor(80, 80, 80));
    palette.setColor(QPalette::Midlight, QColor(70, 70, 70));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(140, 140, 140));
}

/* }}} */

VoidBase* VoidColorStyle::GetProxyStyle(const VoidColorStyle::StyleType& type)
{
    switch (type)
    {
        case VoidColorStyle::StyleType::Obsidian:
            return new Obsidian;
        case VoidColorStyle::StyleType::SakuraPink:
            return new SakuraPink;
        case VoidColorStyle::StyleType::NightBlue:
            return new NightBlue;
        case VoidColorStyle::StyleType::DefaultWhite:
            return new VoidWhite;
        case VoidColorStyle::StyleType::VoidGray:
            return new VoidGray;
        default:
            return new VoidDark;
    }
}

VOID_NAMESPACE_CLOSE
