// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "ListDelegate.h"
#include "VoidUi/VoidStyle.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

const int MAX_THUMBNAIL_WIDTH = 80;
const int MAX_THUMBNAIL_HEIGHT = 50;

MediaItemDelegate::MediaItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void MediaItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /**
     * The main Rect for the Item will be divided into 5 sub sections 
     * -------------------------------------------------------------
     * |               |    Name                    |    Extension |
     * |   Thumbnail   |-------------------------------------------|
     * |               |    1001 - 1010             |        24fps |
     * -------------------------------------------------------------
     */

    /* Base Rect */
    QRect rect = option.rect;

    painter->save();

    /* Default background */
    painter->fillRect(rect, VOID_DARK_BG_COLOR);

    /* Selected */
    if (option.state & QStyle::State_Selected)
    {
        /* Gradient */
        QLinearGradient gradient(0, 0, rect.width(), 0);
        gradient.setColorAt(0, VOID_DARK_BG_COLOR);
        gradient.setColorAt(1, VOID_DARK_HIGHLIGHT_GREEN);
        
        painter->save();

        /* Draw the Background */
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        /* Draw the right indicator rect */
        painter->fillRect(QRect(rect.width() - 4, rect.top(), 4, rect.height()), VOID_HIGHLIGHT_GREEN);

        painter->restore();
    }

    painter->restore();

    /* Save the painter for restoring later */
    painter->save();

    /* Side Bar */
    QRect siderect = QRect(rect.left(), rect.top(), 6, rect.height());
    painter->fillRect(siderect, VOID_GRAY_COLOR);

    /* Thumbnail */
    QRect thumbrect = QRect(rect.left() + 10, rect.top() + 5, MAX_THUMBNAIL_WIDTH, MAX_THUMBNAIL_HEIGHT);
    QPixmap p = index.data(static_cast<int>(MediaModel::MRoles::Thumbnail)).value<QPixmap>();
    QPixmap scaled = p.scaled(MAX_THUMBNAIL_WIDTH, thumbrect.height(), Qt::KeepAspectRatio);

    /* Calculate the point from which the image needs to start getting drawn as to keep it's aspect */
    int x = thumbrect.left() + (MAX_THUMBNAIL_WIDTH - scaled.width()) / 2;
    int y = thumbrect.top() + (MAX_THUMBNAIL_HEIGHT - scaled.height()) / 2;

    /* Draw the pixmap at the calculated coords */
    painter->drawPixmap(x, y, scaled);

    int thumbright = thumbrect.right() + 5;
    int halfheight = rect.height() / 2;

    int namewidth = rect.width() - (thumbrect.width() + 70);

    /* Name */
    QRect namerect = QRect(thumbright, rect.top(), namewidth, halfheight);
    QString name = index.data(static_cast<int>(MediaModel::MRoles::Name)).toString();
    painter->drawText(namerect, Qt::AlignLeft | Qt::AlignVCenter, name);

    /* Extension */
    QRect extrect = QRect(namerect.right(), rect.top(), 46, halfheight);
    QString extension = index.data(static_cast<int>(MediaModel::MRoles::Extension)).toString();
    painter->drawText(extrect, Qt::AlignRight | Qt::AlignVCenter, extension);

    /* Frame range */
    QRect rangerect = QRect(thumbright, namerect.bottom(), namewidth, halfheight);
    QString framerange = index.data(static_cast<int>(MediaModel::MRoles::FrameRange)).toString();
    painter->drawText(rangerect, Qt::AlignLeft | Qt::AlignVCenter, framerange);

    /* Framerate */
    QRect fpsrect = QRect(namerect.right(), extrect.bottom(), 46, halfheight);
    QString framerate = index.data(static_cast<int>(MediaModel::MRoles::Framerate)).toString();
    painter->drawText(fpsrect, Qt::AlignRight | Qt::AlignVCenter, framerate);

    /* Restore for other use */
    painter->restore();
}

QSize MediaItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 60);
}

VOID_NAMESPACE_CLOSE
