// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "ThumbnailDelegate.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

constexpr int MAX_THUMBNAIL_WIDTH = 130;
constexpr int MAX_THUMBNAIL_HEIGHT = 100;

MediaThumbnailDelegate::MediaThumbnailDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void MediaThumbnailDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /**
     * The main Rect for the Item will be divided into 5 sub sections 
     * --------------------------
     * |                        |
     * |                        |
     * |        Thumbnail       |
     * |                        |
     * |                        |
     * --------------------------
     * | Name       | Extension |
     * |------------------------|
     * | 1001 - 1010    | 24fps |
     * --------------------------
     */

    /* Base Rect */
    QRect rect = option.rect;

    /* Size of the Grid Item */
    // int width = 150;
    int height = rect.height();

    painter->save();

    /* Default background */
    QColor bg = option.palette.color(QPalette::Window).lighter(150);
    painter->fillRect(rect, bg);

    /* Selected */
    if (option.state & QStyle::State_Selected)
    {
        /* Gradient */
        QLinearGradient gradient(rect.left(), rect.top(), rect.left() + 150, rect.top());
        gradient.setColorAt(0, bg);
        gradient.setColorAt(1, option.palette.color(QPalette::Highlight).darker(180));
        
        painter->save();

        /* Draw the Background */
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        /* Draw the right indicator rect */
        painter->fillRect(QRect(rect.left() + (150 - 4), rect.top(), 4, height), option.palette.color(QPalette::Highlight));

        painter->restore();
    }

    painter->restore();

    /* Save the painter for restoring later */
    painter->save();

    /* Side Bar */
    painter->fillRect(QRect(rect.left(), rect.top(), 6, height), bg.lighter(150));

    const int left = rect.left() + 10;

    /* Thumbnail */
    const QRect thumbrect = QRect(left, rect.top() + 5, MAX_THUMBNAIL_WIDTH, MAX_THUMBNAIL_HEIGHT);   // Square of 130 x 100;
    QPixmap p = index.data(static_cast<int>(MediaModel::MRoles::Thumbnail)).value<QPixmap>();
    QPixmap scaled = p.scaled(MAX_THUMBNAIL_WIDTH, thumbrect.height(), Qt::KeepAspectRatio);

    /* Calculate the point from which the image needs to start getting drawn as to keep it's aspect */
    int x = left + (MAX_THUMBNAIL_WIDTH - scaled.width()) * 0.5;
    int y = thumbrect.top() + (MAX_THUMBNAIL_HEIGHT - scaled.height()) * 0.5;

    /* Draw the pixmap at the calculated coords */
    painter->drawPixmap(x, y, scaled);

    int thumbbottom = thumbrect.bottom() + 5;

    /* Name */
    const QRect namerect = QRect(left, thumbrect.bottom(), 100, 20);
    painter->drawText(
        namerect,
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Name)).toString()
    );

    /* Extension */
    const QRect extrect = QRect(namerect.right(), thumbrect.bottom(), 30, 20);
    painter->drawText(
        extrect,
        Qt::AlignRight | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Extension)).toString()
    );

    /* Frame range */
    const QRect rangerect = QRect(left, namerect.bottom(), 80, 20);
    painter->drawText(
        rangerect,
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::FrameRange)).toString()
    );

    /* Framerate */
    painter->drawText(
        QRect(rangerect.right(), extrect.bottom(), 50, 20),
        Qt::AlignRight | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Framerate)).toString()
    );

    /* Restore for other use */
    painter->restore();
}

QSize MediaThumbnailDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(150, 146);
}

VOID_NAMESPACE_CLOSE
