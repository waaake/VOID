// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_STYLE_H
#define _VOID_STYLE_H

/* Qt */
#include <QPainter>
#include <QProxyStyle>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/* Color Definitions */
const QColor VOID_DARK_BG_COLOR = {40, 40, 40};
const QColor VOID_SEMI_DARK_COLOR = {25, 25, 25};
const QColor VOID_GRAY_COLOR = {50, 50, 50};

const QColor VOID_DARK_BLUE_COLOR = {30, 80, 70};
const QColor VOID_BLUE_COLOR = {30, 210, 200};

const QColor VOID_PURPLE_COLOR = {130, 110, 190};

const QColor VOID_DARK_HIGHLIGHT_GREEN = {70, 110, 30};
const QColor VOID_HIGHLIGHT_GREEN = {160, 190, 60};
const QColor VOID_FOREGROUND_COLOR = {190, 190, 190};
const QColor VOID_FOREGROUND_DISABLED_COLOR = {80, 80, 80};

class VoidDark : public QProxyStyle
{
public:
    VoidDark();

    // void drawItemText(QPainter* painter, const QRect& rect, int flags, const QPalette& palette, bool enabled, const QString& text, QPalette::ColorRole textRole = QPalette::NoRole) const override;
    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    virtual void polish(QPalette& palette) override;
};

class BlueShore : public VoidDark
{
public:
    BlueShore();
    void polish(QPalette& palette) override;
};

class SakuraPink : public VoidDark
{
public:
    SakuraPink();
    void polish(QPalette& palette) override;
};

class Obsidian : public VoidDark
{
public:
    Obsidian();
    void polish(QPalette& palette) override;
};

namespace VoidColorStyle {

    enum class StyleType
    {
        DefaultDark,
        ShoreBlue,
        SakuraPink,
        Obsidian
    };

    VoidDark* GetProxyStyle(const StyleType& type);

} // namespace VoidColorStyle

VOID_NAMESPACE_CLOSE

#endif // _VOID_STYLE_H
