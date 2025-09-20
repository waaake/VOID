// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_ICON_FORGE_H
#define _VOID_ICON_FORGE_H

/* STD */
#include <map>

/* Qt */
#include <QIcon>
#include <QColor>

/* Internal */
#include "Definition.h"
#include "IconTypes.h"

VOID_NAMESPACE_OPEN

#define _COLOR(x) palette().color(x)
#define _DARK_COLOR(x, y) palette().color(x).darker(y)
#define _LIGHT_COLOR(x, y) palette().color(x).lighter(y)

#ifdef __APPLE__
constexpr int _default_size = 38;
#else
constexpr int _default_size = 18;
#endif

class IconForge
{
private:
    IconForge();

public:
    static IconForge& Instance();
    QPixmap Pixmap(const IconType& icon, int size = 24, const QColor& color = Qt::black);
    QIcon Icon(const IconType& icon, int size = 24, const QColor& color = Qt::black);

    static QPixmap GetPixmap(const IconType& icon, const QColor& color = Qt::black, int size = _default_size);
    static QIcon GetIcon(const IconType& icon, const QColor& color = Qt::black, int size = _default_size);

private: /* Members */  
    QString m_FontFamily;
    std::map<QString, QPixmap> m_Cache;

private: /* Methods */
    void InitFont();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_ICON_FORGE_H
