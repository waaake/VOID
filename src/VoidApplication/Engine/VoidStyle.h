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

class VoidBase : public QProxyStyle
{
public:
    VoidBase();

    // void drawItemText(QPainter* painter, const QRect& rect, int flags, const QPalette& palette, bool enabled, const QString& text, QPalette::ColorRole textRole = QPalette::NoRole) const override;
    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
};

class VoidWhite : public VoidBase
{
public:
    VoidWhite();
};

class VoidDark : public VoidBase
{
public:
    VoidDark();
    virtual void polish(QPalette& palette) override;
};

class NightBlue : public VoidBase
{
public:
    NightBlue();
    void polish(QPalette& palette) override;
};

class SakuraPink : public VoidBase
{
public:
    SakuraPink();
    void polish(QPalette& palette) override;
};

class Obsidian : public VoidBase
{
public:
    Obsidian();
    void polish(QPalette& palette) override;
};

class VoidGray : public VoidBase
{
public:
    VoidGray();
    void polish(QPalette& palette) override;
};

namespace VoidColorStyle {

    enum class StyleType
    {
        DefaultDark,
        NightBlue,
        SakuraPink,
        Obsidian,
        DefaultWhite,
        VoidGray
    };

    VoidBase* GetProxyStyle(const StyleType& type);

} // namespace VoidColorStyle

VOID_NAMESPACE_CLOSE

#endif // _VOID_STYLE_H
