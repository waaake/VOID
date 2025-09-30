// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLineEdit>
#include <QPainter>

/* Internal */
#include "ListDelegate.h"
#include "VoidObjects/Models/MediaModel.h"
#include "VoidObjects/Models/PlaylistModel.h"
#include "VoidUi/Engine/IconForge.h"

VOID_NAMESPACE_OPEN

const int MAX_THUMBNAIL_WIDTH = 80;
const int MAX_THUMBNAIL_HEIGHT = 50;

PlaylistItemDelegate::PlaylistItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void PlaylistItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /**
     * The main Rect for the Item will used as is for the name | item count
     * ------------------------------
     * |  [Icon]  Name          123 |
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

    /* Icon */
    QRect iconrect = QRect(rect.left() + 10, rect.top(), 30, rect.height());
    painter->drawPixmap(
        iconrect,
        IconForge::GetPixmap(IconType::icon_playlist_play, option.palette.color(QPalette::Text).darker(140), 28)
    );

    /* Name */
    QRect namerect = QRect(iconrect.right() + 10, rect.top(), rect.width() - 90, rect.height());
    QString name = index.data(static_cast<int>(PlaylistModel::Roles::Name)).toString();
    painter->drawText(namerect, Qt::AlignLeft | Qt::AlignVCenter, name);

    /* Media Count */
    QRect countrect = QRect(namerect.right(), rect.top(), 50, rect.height());
    QString count = QString::number(index.data(static_cast<int>(PlaylistModel::Roles::MediaCount)).toInt());
    painter->drawText(countrect, Qt::AlignLeft | Qt::AlignCenter, count);

    /* Restore for other use */
    painter->restore();
}

QSize PlaylistItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 30);
}

QWidget* PlaylistItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return new QLineEdit(parent);
}

void PlaylistItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
    if (edit)
        edit->setText(index.data(static_cast<int>(PlaylistModel::Roles::Name)).toString());
}

VOID_NAMESPACE_CLOSE
