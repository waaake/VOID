/* Qt */
#include <QPainter>

/* Internal */
#include "ThumbnailDelegate.h"
#include "VoidUi/MediaBridge.h"
#include "VoidUi/VoidStyle.h"

VOID_NAMESPACE_OPEN

const int MAX_THUMBNAIL_WIDTH = 130;
const int MAX_THUMBNAIL_HEIGHT = 100;

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
    int width = 150;
    int height = rect.height();

    painter->save();

    /* Default background */
    painter->fillRect(rect, VOID_DARK_BG_COLOR);

    /* Selected */
    if (option.state & QStyle::State_Selected)
    {
        /* Gradient */
        QLinearGradient gradient(rect.left(), rect.top(), rect.left() + width, rect.top());
        gradient.setColorAt(0, VOID_DARK_BG_COLOR);
        gradient.setColorAt(1, VOID_DARK_HIGHLIGHT_GREEN);
        
        painter->save();

        /* Draw the Background */
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        /* Draw the right indicator rect */
        painter->fillRect(QRect(rect.left() + (width - 4), rect.top(), 4, height), VOID_HIGHLIGHT_GREEN);

        painter->restore();
    }

    painter->restore();

    /* Save the painter for restoring later */
    painter->save();

    /* Side Bar */
    QRect siderect = QRect(rect.left(), rect.top(), 6, height);
    painter->fillRect(siderect, VOID_GRAY_COLOR);

    int left = rect.left() + 10;

    /* Thumbnail */
    QRect thumbrect = QRect(left, rect.top() + 5, MAX_THUMBNAIL_WIDTH, MAX_THUMBNAIL_HEIGHT);   // Square of 130 x 100;
    QPixmap p = index.data(static_cast<int>(MediaModel::MRoles::Thumbnail)).value<QPixmap>();
    QPixmap scaled = p.scaled(MAX_THUMBNAIL_WIDTH, thumbrect.height(), Qt::KeepAspectRatio);

    /* Calculate the point from which the image needs to start getting drawn as to keep it's aspect */
    int x = left + (MAX_THUMBNAIL_WIDTH - scaled.width()) / 2;
    int y = thumbrect.top() + (MAX_THUMBNAIL_HEIGHT - scaled.height()) / 2;

    /* Draw the pixmap at the calculated coords */
    painter->drawPixmap(x, y, scaled);

    int thumbbottom = thumbrect.bottom() + 5;

    /* Name */
    QRect namerect = QRect(left, thumbrect.bottom(), 100, 20);
    QString name = index.data(static_cast<int>(MediaModel::MRoles::Name)).toString();
    painter->drawText(namerect, Qt::AlignLeft | Qt::AlignVCenter, name);

    /* Extension */
    QRect extrect = QRect(namerect.right(), thumbrect.bottom(), 30, 20);
    QString extension = index.data(static_cast<int>(MediaModel::MRoles::Extension)).toString();
    painter->drawText(extrect, Qt::AlignRight | Qt::AlignVCenter, extension);

    /* Frame range */
    QRect rangerect = QRect(left, namerect.bottom(), 80, 20);
    QString framerange = index.data(static_cast<int>(MediaModel::MRoles::FrameRange)).toString();
    painter->drawText(rangerect, Qt::AlignLeft | Qt::AlignVCenter, framerange);

    /* Framerate */
    QRect fpsrect = QRect(rangerect.right(), extrect.bottom(), 50, 20);
    QString framerate = index.data(static_cast<int>(MediaModel::MRoles::Framerate)).toString();
    painter->drawText(fpsrect, Qt::AlignRight | Qt::AlignVCenter, framerate);

    /* Restore for other use */
    painter->restore();
}

QSize MediaThumbnailDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(150, 146);
}

VOID_NAMESPACE_CLOSE
