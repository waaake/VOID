// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

/* Internal */
#include "ListDelegate.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Engine/IconForge.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

constexpr int MAX_THUMBNAIL_WIDTH = 80;
constexpr int MAX_THUMBNAIL_HEIGHT = 50;

#ifdef _VOID_PLATFORM_APPLE
constexpr int ICON_SIZE = 14;
#else
constexpr int ICON_SIZE = 12;
#endif


/* Basic Media Item Delegate {{{ */

BasicMediaItemDelegate::BasicMediaItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
    , m_TagX(0)
    , m_TagY(0)
{
}

bool BasicMediaItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* item, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        const QRect r(m_TagX, m_TagY, ICON_SIZE, ICON_SIZE);
        auto mevent = static_cast<QMouseEvent*>(event);
        #if _QT6_COMPACT
        QPoint pos = mevent->position();
        #else
        QPoint pos = mevent->pos();
        #endif
        
        if (mevent->button() == Qt::LeftButton && r.contains(pos) && index.data(static_cast<int>(MediaModel::MRoles::Tags)).toBool())
            emit tagClicked(index, pos);
    }

    return QStyledItemDelegate::editorEvent(event, item, option, index);    
}

void BasicMediaItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /**
     * The main Rect for the Item will used as is for the name | item count
     * ------------------------------
     * |    Name      1001 - 1200   |
     * ------------------------------
     */

    painter->fillRect(option.rect, option.palette.color(QPalette::Window).lighter(150));

    /* Selected */
    if (option.state & QStyle::State_Selected)
    {   
        /* Gradient */
        QLinearGradient gradient(0, 0, option.rect.width(), 0);
        gradient.setColorAt(0, option.palette.color(QPalette::Window).lighter(150));
        gradient.setColorAt(1, option.palette.color(QPalette::Highlight).darker(180));

        painter->save();

        /* Draw the Background */
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);

        /* Draw the right indicator rect */
        painter->fillRect(
            option.rect.width() - 4,
            option.rect.top(),
            4,
            option.rect.height(),
            option.palette.color(QPalette::Highlight)
        );

        painter->restore();
    }

    /* Side Bar */
    painter->fillRect(
        option.rect.left(),
        option.rect.top(),
        6,
        option.rect.height(),
        option.palette.color(QPalette::Window).lighter(300)
    );

    m_TagX = option.rect.left() + 10;
    m_TagY = option.rect.top() + 4 + ICON_SIZE + 2;

    if (index.data(static_cast<int>(MediaModel::MRoles::Audio)).toBool())
        painter->drawPixmap(
            option.rect.left() + 10, option.rect.top() + 4,
            IconForge::GetPixmap(IconType::icon_volume_up, option.palette.color(QPalette::Text),
            ICON_SIZE
        ));

    if (index.data(static_cast<int>(MediaModel::MRoles::Tags)).toBool())
        painter->drawPixmap(
            m_TagX,
            m_TagY,
            IconForge::GetPixmap(IconType::icon_style, option.palette.color(QPalette::Text), ICON_SIZE)
        );

    /* Name */
    const QRect namerect(option.rect.left() + 30, option.rect.top(), option.rect.right(), option.rect.height());
    painter->drawText(
        namerect,
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Name)).toString()
    );

    /* Frame Range */
    painter->drawText(
        namerect.left(),
        option.rect.top(),
        option.rect.right() - 40,
        option.rect.height(),
        Qt::AlignRight | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::FrameRange)).toString()
    );
}

QSize BasicMediaItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 40);
}

/* }}} */

MediaItemDelegate::MediaItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
    , m_TagX(0)
    , m_TagY(0)
{
}

bool MediaItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* item, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        const QRect r(m_TagX, m_TagY, ICON_SIZE, ICON_SIZE);
        auto mevent = static_cast<QMouseEvent*>(event);
        #if _QT6_COMPACT
        QPoint pos = mevent->position();
        #else
        QPoint pos = mevent->pos();
        #endif
        
        if (mevent->button() == Qt::LeftButton && r.contains(pos) && index.data(static_cast<int>(MediaModel::MRoles::Tags)).toBool())
            emit tagClicked(index, pos);
    }

    return QStyledItemDelegate::editorEvent(event, item, option, index);
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

    /* Default background */
    painter->fillRect(rect, option.palette.color(QPalette::Window).lighter(150));

    /* Selected */
    if (option.state & QStyle::State_Selected)
    {
        /* Gradient */
        QLinearGradient gradient(0, 0, rect.width(), 0);
        gradient.setColorAt(0, option.palette.color(QPalette::Window).lighter(150));
        gradient.setColorAt(1, option.palette.color(QPalette::Highlight).darker(150));
        
        painter->save();

        /* Draw the Background */
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        /* Draw the right indicator rect */
        painter->fillRect(rect.width() - 4, rect.top(), 4, rect.height(), option.palette.color(QPalette::Highlight));

        painter->restore();
    }

    /* Side Bar */
    painter->fillRect(rect.left(), rect.top(), 6, rect.height(), option.palette.color(QPalette::Window).lighter(300));

    /* Thumbnail */
    const QRect thumbrect(rect.left() + 10, rect.top() + 5, MAX_THUMBNAIL_WIDTH, MAX_THUMBNAIL_HEIGHT);
    QPixmap p = index.data(static_cast<int>(MediaModel::MRoles::Thumbnail)).value<QPixmap>();
    QPixmap scaled = p.scaled(MAX_THUMBNAIL_WIDTH, thumbrect.height(), Qt::KeepAspectRatio);

    /* Calculate the point from which the image needs to start getting drawn as to keep it's aspect */
    const int x = thumbrect.left() + (MAX_THUMBNAIL_WIDTH - scaled.width()) * 0.5;
    const int y = thumbrect.top() + (MAX_THUMBNAIL_HEIGHT - scaled.height()) * 0.5;

    m_TagX = thumbrect.left();
    m_TagY = y + ICON_SIZE + 2;

    /* Draw the pixmap at the calculated coords */
    painter->drawPixmap(x, y, scaled);

    const bool audio = index.data(static_cast<int>(MediaModel::MRoles::Audio)).toBool();
    const bool tags = index.data(static_cast<int>(MediaModel::MRoles::Tags)).toBool();

    if (audio || tags)
    {
        QLinearGradient fade(thumbrect.topLeft(), thumbrect.bottomRight());
        fade.setColorAt(0.0, option.palette.color(QPalette::Highlight).darker(180));
        fade.setColorAt(0.05, option.palette.color(QPalette::Highlight).darker(180));
        fade.setColorAt(0.3, QColor(0, 0, 0, 0));
        
        painter->fillRect(thumbrect, fade);
    }
    
    if (audio)
        painter->drawPixmap(x, y, IconForge::GetPixmap(IconType::icon_volume_up, option.palette.color(QPalette::Text), ICON_SIZE));

    if (tags)
        painter->drawPixmap(m_TagX, m_TagY, IconForge::GetPixmap(IconType::icon_style, option.palette.color(QPalette::Text), ICON_SIZE));

    const int thumbright = thumbrect.right() + 5;
    const int halfheight = rect.height() * 0.5;
    const int namewidth = rect.width() - (thumbrect.width() + 70);

    /* Name */
    const QRect namerect(thumbright, rect.top(), namewidth, halfheight);
    painter->drawText(
        namerect,
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Name)).toString()
    );

    /* Extension */
    const QRect extrect(namerect.right(), rect.top(), 46, halfheight);
    painter->drawText(
        extrect,
        Qt::AlignRight | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Extension)).toString()
    );

    /* Frame range */
    painter->drawText(
        thumbright,
        namerect.bottom(),
        namewidth,
        halfheight,
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::FrameRange)).toString()
    );

    /* Framerate */
    painter->drawText(
        namerect.right(),
        extrect.bottom(),
        46,
        halfheight,
        Qt::AlignRight | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Framerate)).toString()
    );
}

QSize MediaItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 60);
}

VOID_NAMESPACE_CLOSE
