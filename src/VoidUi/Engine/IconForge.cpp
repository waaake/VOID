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

// QIcon IconForge::Icon(const IconType& icon, int size)
// {
//     return Icon(icon, size,)
// }

QIcon IconForge::Icon(const IconType& icon, int size, const QColor& color)
{
    QString key = QString("%1_%2_%3").arg(static_cast<char16_t>(icon)).arg(size).arg(color.name());

    /* Already Exists in Cache */
    if (m_Cache.find(key) != m_Cache.end())
    {
        VOID_LOG_INFO("FOUND in Cache.");
        return m_Cache[key];
    }

    VOID_LOG_INFO("Constructing Icon.");
    QFont font(m_FontFamily);
    font.setPointSize(size);
    // font.setBold(true);

    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setFont(font);
    painter.setPen(color);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, QChar(static_cast<char16_t>(icon)));

    pixmap.save("/mnt/c/Skids/zework/s_imgs/test.png");

    QIcon icon_(pixmap);

    m_Cache[key] = icon_;
    return icon_;
}

QIcon GetIcon(const IconType& icon, const QColor& color, int size)
{
    return IconForge::Instance().Icon(icon, size, color);
}

VOID_NAMESPACE_CLOSE
