// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

/* Internal */
#include "ThumbnailDelegate.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Engine/IconForge.h"

VOID_NAMESPACE_OPEN

constexpr int MAX_THUMBNAIL_WIDTH = 120;
constexpr int MAX_THUMBNAIL_HEIGHT = 100;

#ifdef _VOID_PLATFORM_APPLE
constexpr int ICON_SIZE = 16;
#else
constexpr int ICON_SIZE = 12;
#endif

MediaThumbnailDelegate::MediaThumbnailDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
    , m_TagX(0)
    , m_TagY(0)
{
}

bool MediaThumbnailDelegate::editorEvent(QEvent* event, QAbstractItemModel* item, const QStyleOptionViewItem& option, const QModelIndex& index)
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
    // int height = rect.height();

    /* Default background */
    painter->fillRect(rect, option.palette.color(QPalette::Window).lighter(150));

    /* Selected */
    if (option.state & QStyle::State_Selected)
    {
        /* Gradient */
        QLinearGradient gradient(rect.left(), rect.top(), rect.left() + 150, rect.top());
        gradient.setColorAt(0, option.palette.color(QPalette::Window).lighter(150));
        gradient.setColorAt(1, option.palette.color(QPalette::Highlight).darker(180));
        
        painter->save();

        /* Draw the Background */
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        /* Draw the right indicator rect */
        painter->fillRect(rect.left() + (150 - 4), rect.top(), 4, rect.height(), option.palette.color(QPalette::Highlight));
        painter->restore();
    }

    /* Side Bar */
    painter->fillRect(rect.left(), rect.top(), ICON_SIZE + 6, rect.height(), option.palette.color(QPalette::Window).lighter(220));

    const int left = rect.left() + ICON_SIZE + 10;

    /* Thumbnail */
    const QRect thumbrect(left, rect.top() + 5, MAX_THUMBNAIL_WIDTH, MAX_THUMBNAIL_HEIGHT);   // Rect of 120 x 100;
    QPixmap p = index.data(static_cast<int>(MediaModel::MRoles::Thumbnail)).value<QPixmap>();
    QPixmap scaled = p.scaled(MAX_THUMBNAIL_WIDTH, thumbrect.height(), Qt::KeepAspectRatio);

    /* Calculate the point from which the image needs to start getting drawn as to keep it's aspect */
    int x = left + (MAX_THUMBNAIL_WIDTH - scaled.width()) * 0.5;
    int y = thumbrect.top() + (MAX_THUMBNAIL_HEIGHT - scaled.height()) * 0.5;

    m_TagX = rect.left() + 2;
    m_TagY = rect.top() + ICON_SIZE + 4;

    /* Draw the pixmap at the calculated coords */
    painter->drawPixmap(x, y, scaled);

    const bool audio = index.data(static_cast<int>(MediaModel::MRoles::Audio)).toBool();
    const bool tags = index.data(static_cast<int>(MediaModel::MRoles::Tags)).toBool();

    painter->drawPixmap(rect.left() + 2, rect.top() + 2, IconForge::GetPixmap(
        IconType::icon_volume_up,
        audio ? option.palette.color(QPalette::Text) : option.palette.color(QPalette::Window).lighter(280),
        ICON_SIZE
    ));

    painter->drawPixmap(m_TagX, m_TagY, IconForge::GetPixmap(
        IconType::icon_style,
        tags ? option.palette.color(QPalette::Text) : option.palette.color(QPalette::Window).lighter(280),
        ICON_SIZE
    ));

    /* Name */
    const QRect namerect(left, thumbrect.bottom(), 90, 20);
    painter->drawText(
        namerect,
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Name)).toString()
    );

    /* Extension */
    const QRect extrect(namerect.right(), thumbrect.bottom(), 30, 20);
    painter->drawText(
        extrect,
        Qt::AlignRight | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Extension)).toString()
    );

    /* Frame range */
    const QRect rangerect(left, namerect.bottom(), 70, 20);
    painter->drawText(
        rangerect,
        Qt::AlignLeft | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::FrameRange)).toString()
    );

    /* Framerate */
    painter->drawText(
        rangerect.right(),
        extrect.bottom(),
        50,
        20,
        Qt::AlignRight | Qt::AlignVCenter,
        index.data(static_cast<int>(MediaModel::MRoles::Framerate)).toString()
    );
}

QSize MediaThumbnailDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(150, 146);
}

VOID_NAMESPACE_CLOSE
