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

class IconForge
{
private:
    IconForge();

public:
    static IconForge& Instance();
    QIcon Icon(const IconType& icon, int size = 24, const QColor& color = Qt::black);
    // QIcon Icon(const IconType& icon, int size = 24);
    static QIcon GetIcon(const IconType& icon, const QColor& color = Qt::black, int size = 26);

private: /* Members */  
    QString m_FontFamily;
    std::map<QString, QIcon> m_Cache;

private: /* Methods */
    void InitFont();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_ICON_FORGE_H
