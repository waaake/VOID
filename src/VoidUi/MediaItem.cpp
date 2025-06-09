/* Qt */
#include <QStyle>
#include <QPainter>
#include <QMouseEvent>

/* Internal */
#include "MediaItem.h"
#include "VoidStyle.h"
#include "VoidCore/VoidTools.h"

static const int THUMBNAIL_SIZE = 60;
static const int ICON_SIZE = 20;

VOID_NAMESPACE_OPEN

VoidMediaItem::VoidMediaItem(const SharedMediaClip& media, QWidget* parent)
    : QFrame(parent)
    , m_Selected(false)
    , m_Playing(false)
    , m_Clip(media)
    , m_AssociatedColor(VOID_PURPLE_COLOR)
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
    const VoidImageData* im = m_Clip->FirstFrameData().ImageData();
    QImage::Format format = (im->Channels() == 3) ? QImage::Format_RGB888 : QImage::Format_RGBA8888;

    /* Resize the Frame to a thumbnail size */
    QImage scaled = QImage(im->Data(), im->Width(), im->Height(), format).scaled(
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
