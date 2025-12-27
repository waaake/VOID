// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "ListDelegate.h"
#include "VoidUi/Engine/Globals.h"
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
    QColor bg = index.data(static_cast<int>(ProjectModel::Roles::Active)).toBool() ? option.palette.color(QPalette::Window).lighter(150) : option.palette.color(QPalette::Window).lighter(120);
    painter->fillRect(rect, bg);

    QColor fg = option.palette.color(QPalette::WindowText);

    /* Selected */
    if (option.state & QStyle::State_Selected)
    {        
        painter->save();

        /* Draw the Background */
        painter->setBrush(option.palette.color(QPalette::Highlight).darker(150));
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        /* Draw the right indicator rect */
        painter->fillRect(QRect(rect.width() - 4, rect.top(), 4, rect.height()), option.palette.color(QPalette::Highlight));

        painter->restore();

        if (!UIGlobals::IsDarkTheme())
            fg = option.palette.color(QPalette::HighlightedText);
    }

    painter->restore();

    /* Save the painter for restoring later */
    painter->save();

    painter->setPen(fg);

    /* Side Bar */
    QRect siderect = QRect(rect.left(), rect.top(), 6, rect.height());
    QColor sidecol = index.data(static_cast<int>(ProjectModel::Roles::SaveState)).toBool() ? option.palette.color(QPalette::Disabled, QPalette::Text) : bg;
    // painter->fillRect(siderect, bg.lighter(250));
    painter->fillRect(siderect, sidecol);

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

/* Recent Project Item Delegate {{{ */

RecentProjectItemDelegate::RecentProjectItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void RecentProjectItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /**
     * The main Rect for the Item will used as is for the name
     * ------------------------------
     * | Name.ext      Thurs 18 Dec |
     * ------------------------------
     */
    QRect r = option.rect;
    QRect left = QRect(r.left() + 6 , r.top(), r.width() / 2, r.height());
    QRect right = QRect(left.right(), r.top(), r.width() / 2 - 12, r.height());

    QColor fg = option.palette.color(QPalette::Text);
    QColor bg = option.palette.color(QPalette::Window).lighter(200);

    painter->save();
    painter->setPen(Qt::NoPen);

     /* Selected */
    if (option.state & QStyle::State_Selected)
    {        
        bg = option.palette.color(QPalette::Highlight);
        fg = Qt::black;
    }

    /* Paint the BG */
    painter->setBrush(bg);
    painter->drawRect(r);

    /* Draw Text text */
    painter->setPen(fg);
    painter->drawText(left, Qt::AlignLeft | Qt::AlignVCenter, index.data(static_cast<int>(RecentProjectsModel::Roles::Name)).toString());
    painter->drawText(right, Qt::AlignRight | Qt::AlignVCenter, index.data(static_cast<int>(RecentProjectsModel::Roles::Modification)).toString());

    painter->restore();
}

QSize RecentProjectItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 38);
}

/* }}} */

VOID_NAMESPACE_CLOSE
