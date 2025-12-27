// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "ListDelegate.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

constexpr int MAX_THUMBNAIL_WIDTH = 80;
constexpr int MAX_THUMBNAIL_HEIGHT = 50;

/* Basic Media Item Delegate {{{ */

BasicMediaItemDelegate::BasicMediaItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void BasicMediaItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /**
     * The main Rect for the Item will used as is for the name | item count
     * ------------------------------
     * |    Name      1001 - 1200   |
     * ------------------------------
     */

    /* Base Rect */
    QRect rect = option.rect;

    painter->save();

    /* Default background */
    QColor bg = option.palette.color(QPalette::Window).lighter(150);
    painter->fillRect(rect, bg);

    /* Selected */
    if (option.state & QStyle::State_Selected)
    {   
        /* Gradient */
        QLinearGradient gradient(0, 0, rect.width(), 0);
        gradient.setColorAt(0, bg);
        gradient.setColorAt(1, option.palette.color(QPalette::Highlight).darker(180));

        painter->save();

        /* Draw the Background */
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        /* Draw the right indicator rect */
        painter->fillRect(QRect(rect.width() - 4, rect.top(), 4, rect.height()), option.palette.color(QPalette::Highlight));

        painter->restore();
    }

    painter->restore();

    /* Save the painter for restoring later */
    painter->save();

    /* Side Bar */
    QRect siderect = QRect(rect.left(), rect.top(), 6, rect.height());
    painter->fillRect(siderect, bg.lighter(250));

    /* Name */
    QRect namerect = QRect(rect.left() + 10, rect.top(), rect.right(), rect.height());
    QString name = index.data(static_cast<int>(MediaModel::MRoles::Name)).toString();
    painter->drawText(namerect, Qt::AlignLeft | Qt::AlignVCenter, name);

    /* Frame Range */
    QRect countrect = QRect(namerect.left(), rect.top(), rect.right() - 20, rect.height());
    QString count = index.data(static_cast<int>(MediaModel::MRoles::FrameRange)).toString();
    painter->drawText(countrect, Qt::AlignRight | Qt::AlignVCenter, count);

    /* Restore for other use */
    painter->restore();
}

QSize BasicMediaItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 40);
}

/* }}} */

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
    QColor bg = option.palette.color(QPalette::Window).lighter(150);
    painter->fillRect(rect, bg);

    /* Selected */
    if (option.state & QStyle::State_Selected)
    {
        /* Gradient */
        QLinearGradient gradient(0, 0, rect.width(), 0);
        gradient.setColorAt(0, bg);
        gradient.setColorAt(1, option.palette.color(QPalette::Highlight).darker(150));
        
        painter->save();

        /* Draw the Background */
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        /* Draw the right indicator rect */
        painter->fillRect(QRect(rect.width() - 4, rect.top(), 4, rect.height()), option.palette.color(QPalette::Highlight));

        painter->restore();
    }

    painter->restore();

    /* Save the painter for restoring later */
    painter->save();

    /* Side Bar */
    QRect siderect = QRect(rect.left(), rect.top(), 6, rect.height());
    painter->fillRect(siderect, bg.lighter(150));

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
