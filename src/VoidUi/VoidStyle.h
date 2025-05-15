#ifndef _VOID_STYLE_H
#define _VOID_STYLE_H

/* Qt */
#include <QPainter>
#include <QProxyStyle>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VoidDark : public QProxyStyle
{
public:
    VoidDark();

    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    void polish(QPalette& palette) override;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_STYLE_H
