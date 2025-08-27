// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "ListDelegate.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

const int MAX_THUMBNAIL_WIDTH = 80;
const int MAX_THUMBNAIL_HEIGHT = 50;

ProjectItemDelegate::ProjectItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void ProjectItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /**
     * The main Rect for the Item will used as is for the name
     * ------------------------------
     * |    Name                    |
     * ------------------------------
     */

    /* Base Rect */
    QRect rect = option.rect;

    painter->save();

    /* Default background */
    QColor bg = index.data(static_cast<int>(ProjectModel::Roles::Active)).toBool() ? option.palette.color(QPalette::Window).lighter(250) : option.palette.color(QPalette::Window).lighter(150);
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
    QString name = index.data(static_cast<int>(ProjectModel::Roles::Name)).toString();
    painter->drawText(namerect, Qt::AlignLeft | Qt::AlignVCenter, name);

    /* Restore for other use */
    painter->restore();
}

QSize ProjectItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 20);
}

VOID_NAMESPACE_CLOSE
