// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "ListDelegate.h"
#include "VoidObjects/Models/MediaModel.h"
#include "VoidObjects/Models/PlaylistModel.h"

VOID_NAMESPACE_OPEN

const int MAX_THUMBNAIL_WIDTH = 80;
const int MAX_THUMBNAIL_HEIGHT = 50;

/* Playlist Item Delegate {{{ */

PlaylistItemDelegate::PlaylistItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void PlaylistItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /**
     * The main Rect for the Item will used as is for the name | item count
     * ------------------------------
     * |    Name                123 |
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
        painter->save();

        /* Draw the Background */
        painter->setBrush(option.palette.color(QPalette::Highlight).darker(180));
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
    QString name = index.data(static_cast<int>(PlaylistModel::Roles::Name)).toString();
    painter->drawText(namerect, Qt::AlignLeft | Qt::AlignVCenter, name);

    /* Media Count */
    QRect countrect = QRect(namerect.left(), rect.top(), rect.right() - 20, rect.height());
    QString count = QString::number(index.data(static_cast<int>(PlaylistModel::Roles::MediaCount)).toInt());
    painter->drawText(countrect, Qt::AlignRight | Qt::AlignVCenter, count);

    /* Restore for other use */
    painter->restore();
}

QSize PlaylistItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 40);
}

/* }}} */

/* Playlist Media Item Delegate {{{ */

PlaylistMediaItemDelegate::PlaylistMediaItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void PlaylistMediaItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
        painter->save();

        /* Draw the Background */
        painter->setBrush(option.palette.color(QPalette::Highlight).darker(180));
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

QSize PlaylistMediaItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 40);
}

/* }}} */

VOID_NAMESPACE_CLOSE
