// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "VersionDelegate.h"
#include "VoidObjects/Models/EntityModel.h"

VOID_NAMESPACE_OPEN

constexpr int MAX_THUMBNAIL_WIDTH = 80;
constexpr int MAX_THUMBNAIL_HEIGHT = 50;

#ifdef _VOID_PLATFORM_APPLE
constexpr int ICON_SIZE = 14;
#else
constexpr int ICON_SIZE = 12;
#endif

#define _ENTITY_TYPE(x) static_cast<ProjectEntity::Type>(x.data(static_cast<int>(EntityModel::MRoles::Type)).toInt())

MediaVersionDelegate::MediaVersionDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void MediaVersionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /**
     * The main Rect for the Item will be divided into 5 sub sections 
     * -------------------------------------------------------------
     * |               |    Name                    |      Version |
     * |   Thumbnail   |-------------------------------------------|
     * |               |    1001 - 1010             |        24fps |
     * -------------------------------------------------------------
     */

    // Base Rect
    QRect rect = option.rect;
    // Background
    painter->fillRect(rect, option.palette.color(QPalette::Base).darker(150));

    /// Selected
    if (option.state & QStyle::State_Selected)
    {
        QLinearGradient gradient(0, 0, rect.width(), 0);
        gradient.setColorAt(0, option.palette.color(QPalette::Base).darker(150));
        gradient.setColorAt(1, option.palette.color(QPalette::Highlight).darker(150));
        
        painter->save();

        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        painter->fillRect(rect.width() - 3, rect.top(), 4, rect.height(), option.palette.color(QPalette::Highlight));

        painter->restore();
    }

    /// Thumbnail
    const QRect thumbrect(rect.left() + 6, rect.top() + 5, MAX_THUMBNAIL_WIDTH, MAX_THUMBNAIL_HEIGHT);
    QPixmap p = index.data(static_cast<int>(EntityModel::MRoles::Thumbnail)).value<QPixmap>();
    QPixmap scaled = p.scaled(MAX_THUMBNAIL_WIDTH, thumbrect.height(), Qt::KeepAspectRatio);

    const int x = thumbrect.left() + (MAX_THUMBNAIL_WIDTH - scaled.width()) * 0.5;
    const int y = thumbrect.top() + (MAX_THUMBNAIL_HEIGHT - scaled.height()) * 0.5;
    painter->drawPixmap(x, y, scaled);

    const int channels = index.data(static_cast<int>(EntityModel::MRoles::Channels)).toInt();
    if (channels == 3)
    {
        constexpr int w = (ICON_SIZE + 6) * 0.333334;
        painter->fillRect(rect.left(), rect.bottom() - 1, w, 2, QColor(255, 0, 0));
        painter->fillRect(rect.left() + w, rect.bottom() - 1, w, 2, QColor(0, 255, 0));
        painter->fillRect(rect.left() + 2 * w, rect.bottom() - 1, w, 2, QColor(0, 0, 255));
    }
    else if (channels == 4)
    {
        constexpr int w = (ICON_SIZE + 6) * 0.25;
        painter->fillRect(rect.left(), rect.bottom() - 1, w, 2, QColor(255, 0, 0));
        painter->fillRect(rect.left() + w, rect.bottom() - 1, w, 2, QColor(0, 255, 0));
        painter->fillRect(rect.left() + 2 * w, rect.bottom() - 1, w, 2, QColor(0, 0, 255));
        painter->fillRect(rect.left() + 3 * w, rect.bottom() - 1, w, 2, QColor(255, 255, 255));
    }

    const int thumbright = thumbrect.right() + 5;
    const int halfheight = rect.height() * 0.5;
    const int namewidth = rect.width() - (thumbrect.width() + 90);

    // Name
    const QRect namerect(thumbright, rect.top(), namewidth, halfheight);
    painter->drawText(
        namerect,
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(static_cast<int>(EntityModel::MRoles::Name)).toString()
    );

    // Version
    const QRect extrect(namerect.right(), rect.top(), 60, halfheight);
    painter->drawText(
        extrect,
        Qt::AlignRight | Qt::AlignVCenter,
        index.data(static_cast<int>(EntityModel::MRoles::VersionName)).toString()
    );

    // Frame range
    painter->drawText(
        QRect(thumbright, namerect.bottom(), namewidth, halfheight),
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(static_cast<int>(EntityModel::MRoles::FrameRange)).toString()
    );

    // Framerate
    painter->drawText(
        QRect(namerect.right(), extrect.bottom(), 60, halfheight),
        Qt::AlignRight | Qt::AlignVCenter,
        index.data(static_cast<int>(EntityModel::MRoles::Framerate)).toString()
    );
}

QSize MediaVersionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 60);
}

VOID_NAMESPACE_CLOSE
