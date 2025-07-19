/* Qt */
#include <QStyle>
#include <QPainter>
#include <QMouseEvent>

/* Internal */
#include "MediaBridge.h"
#include "MediaItem.h"
#include "VoidStyle.h"
#include "VoidCore/VoidTools.h"

static const int THUMBNAIL_SIZE = 60;
static const int ICON_SIZE = 20;

VOID_NAMESPACE_OPEN

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
        // painter->fillRect(rect, option.palette.highlight());
        // painter->fillRect(rect, QColor(160, 190, 60));

        // QColor selectionIndicator = VOID_BLUE_COLOR;

        /* Gradient */
        QLinearGradient gradient(0, 0, rect.width(), 0);
        gradient.setColorAt(0, VOID_DARK_BG_COLOR);
        gradient.setColorAt(1, VOID_DARK_BLUE_COLOR);
        
        painter->save();

        /* Draw the Background */
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        /* Draw the right indicator rect */
        painter->fillRect(QRect(rect.width() - 4, rect.top(), 4, rect.height()), VOID_BLUE_COLOR);

        painter->restore();
    }

    painter->restore();

    /* Save the painter for restoring later */
    painter->save();

    /* Side Bar */
    QRect siderect = QRect(rect.left(), rect.top(), 6, rect.height());
    painter->fillRect(siderect, VOID_GRAY_COLOR);

    /* Thumbnail */
    QRect thumbrect = QRect(rect.left() + 10, rect.top() + 5, 80, 50);
    QPixmap p = index.data(static_cast<int>(MediaModel::MRoles::Thumbnail)).value<QPixmap>();
    painter->drawPixmap(thumbrect, p.scaled(thumbrect.width(), thumbrect.height(), Qt::KeepAspectRatio));

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

VoidMediaItem::VoidMediaItem(const SharedMediaClip& media, QWidget* parent)
    : QFrame(parent)
    , m_Selected(false)
    , m_Playing(false)
    , m_Clip(media)
    , m_AssociatedColor(media->Color())
{
    /* Build the layout */
    Build();

    /* Setup the UI */
    Setup();

    /* TODO: Check if we actually need this on the clip -- Maybe we do? */
    /* Fetch the associated color from the clip based on what track it's associated with */
    connect(m_Clip.get(), &MediaClip::updated, this, [this]() { m_AssociatedColor = m_Clip->Color(); update(); });
}

VoidMediaItem::~VoidMediaItem()
{
}

void VoidMediaItem::mousePressEvent(QMouseEvent* event)
{
    /* The clicked signal and selected signal gets emitted when the Mouse Left Click Happens */
    if (event->button() == Qt::LeftButton)
    {
        /* Emit that this item was clicked on */
        emit clicked(this);
    
        /* This holds the value as to if any other selection needs to be retained or not */
        bool clear = event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::ShiftModifier ? false : true;
        
        /* Emit that the item has been selected */
        emit selected(this, clear);
    }
}

void VoidMediaItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit doubleClicked(this);
}

void VoidMediaItem::paintEvent(QPaintEvent* event)
{
    /* Painter */
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    /* Default colors */
    QColor leftIndicator = m_Playing ? m_AssociatedColor : VOID_GRAY_COLOR;
    QColor selectionIndicator = m_Selected ? VOID_BLUE_COLOR : VOID_DARK_BG_COLOR;

    /* Gradient */
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0, VOID_DARK_BG_COLOR);
    gradient.setColorAt(1, m_Selected ? VOID_DARK_BLUE_COLOR : VOID_DARK_BG_COLOR);
    
    /* Draw the Background */
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    /* Draw the Left indicator rect */
    painter.setBrush(leftIndicator);
    painter.drawRect(0, 0, 6, height());

    /* Draw the right indicator rect */
    painter.setBrush(selectionIndicator);
    painter.drawRect(width() - 4, 0, 4, height());
}

void VoidMediaItem::Build()
{
    /* Intitalise */
    /* Base Layout */
    m_Layout = new QHBoxLayout(this);
    
    m_RtLayout = new QVBoxLayout;
    m_IntTpLayout = new QHBoxLayout;
    m_IntBtLayout = new QHBoxLayout;
    m_IconLayout = new QVBoxLayout;

    m_NameLabel = new QLabel;
    m_FramerateLabel = new QLabel;
    m_RangeLabel = new QLabel;
    m_TypeLabel = new QLabel;
    m_Thumbnail = new QLabel;

    /* Add to the Layout */
    m_IntBtLayout->addWidget(m_RangeLabel);
    m_IntBtLayout->insertStretch(1, 1);
    m_IntBtLayout->addWidget(m_FramerateLabel);

    m_IntTpLayout->addWidget(m_NameLabel);
    m_IntTpLayout->insertStretch(1, 1);
    m_IntTpLayout->addWidget(m_TypeLabel);

    m_RtLayout->addLayout(m_IntTpLayout);
    m_RtLayout->addLayout(m_IntBtLayout);

    m_Layout->addWidget(m_Thumbnail);
    m_Layout->addLayout(m_RtLayout);
    m_Layout->addLayout(m_IconLayout);
}

void VoidMediaItem::Setup()
{
    /* Update the values from the media */
    Update();

    /* Fixed Height */
    setFixedHeight(60);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    /* Focus policy set to receive focus when clicked */
    setFocusPolicy(Qt::ClickFocus);
}

std::string VoidMediaItem::GetFramerate() const
{
    return Tools::to_trimmed_string(m_Clip->Framerate()) + "fps";
}

std::string VoidMediaItem::GetRange() const
{
    return std::to_string(m_Clip->FirstFrame()) + "-" + std::to_string(m_Clip->LastFrame());
}

std::string VoidMediaItem::GetName() const
{
    return m_Clip->Name();
}

std::string VoidMediaItem::GetType() const
{
    return m_Clip->Extension();
}

QPixmap VoidMediaItem::GetThumbnail() const
{   
    /* Grab the pointer to the image data for the first frame to be used as a thumbnail */
    const SharedPixels im = m_Clip->FirstImage();
    QImage::Format format = (im->Channels() == 3) ? QImage::Format_RGB888 : QImage::Format_RGBA8888;

    /* Resize the Frame to a thumbnail size */
    QImage scaled = QImage(im->ThumbnailPixels(), im->Width(), im->Height(), format).scaled(
            THUMBNAIL_SIZE, THUMBNAIL_SIZE, Qt::KeepAspectRatio
        );

    return QPixmap::fromImage(scaled);
}

void VoidMediaItem::SetPlaying(bool play)
{
    /* Update the play state */
    m_Playing = play;

    /* Repaint */
    update();
}

void VoidMediaItem::SetSelected(bool selected)
{
    /* Update the selection state */
    m_Selected = selected;

    /* Repaint */
    update();
}

void VoidMediaItem::Update()
{
    /* Thumbnail Icon */
    m_Thumbnail->setPixmap(GetThumbnail());
    
    /* Update Labels */
    m_NameLabel->setText(GetName().c_str());
    m_TypeLabel->setText(GetType().c_str());
    m_RangeLabel->setText(GetRange().c_str());
    m_FramerateLabel->setText(GetFramerate().c_str());
}

VOID_NAMESPACE_CLOSE
