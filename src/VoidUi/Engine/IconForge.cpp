// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QApplication>
#include <QFontDatabase>
#include <QPainter>
#include <QPixmap>

/* Internal */
#include "IconForge.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

IconForge::IconForge()
{
    InitFont();
}

IconForge& IconForge::Instance()
{
    static IconForge instance;
    return instance;
}

void IconForge::InitFont()
{
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/MaterialSymbolsSharp-Regular.ttf");
    if (fontId == -1)
    {
        VOID_LOG_WARN("Failed to Load Icon Font");
    }
    else
    {
        m_FontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        VOID_LOG_INFO("Icon Font Loaded {0}", m_FontFamily.toStdString());
    }
}

QPixmap IconForge::Pixmap(const IconType& icon, int size, const QColor& color)
{
    QString key = QString("%1_%2_%3").arg(static_cast<char16_t>(icon)).arg(size).arg(color.name());

    /* Already Exists in Cache */
    if (m_Cache.find(key) != m_Cache.end())
        return m_Cache[key];

    QFont font(m_FontFamily);
    font.setPointSize(size);
    // font.setBold(true);

    QPixmap pixmap(size + 2, size + 2);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setFont(font);
    painter.setPen(color);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, QChar(static_cast<char16_t>(icon)));

    m_Cache[key] = pixmap;
    return pixmap;
}

QIcon IconForge::Icon(const IconType& icon, int size, const QColor& color)
{
    return QIcon(Pixmap(icon, size, color));
}

QPixmap IconForge::GetPixmap(const IconType& icon, const QColor& color, int size)
{
    static IconForge instance;
    return instance.Pixmap(icon, size, color);
}

QIcon IconForge::GetIcon(const IconType& icon, const QColor& color, int size)
{
    static IconForge instance;
    return instance.Icon(icon, size, color);
}

VOID_NAMESPACE_CLOSE
