/* Qt */
#include <QStyle>

/* Internal */
#include "MediaItem.h"
#include "VoidCore/VoidTools.h"

static const int THUMBNAIL_SIZE = 60;
static const int ICON_SIZE = 20;

VOID_NAMESPACE_OPEN

/* Stylesheets */
const std::string VoidMediaItem::m_DefaultSS = "\
            QFrame {\
                background-color: #80C3D5;\
                color: #000000;\
            }\
        ";

const std::string VoidMediaItem::m_SelectedSS = "\
            QFrame {\
                background-color: #DAE589;\
                color: #000000;\
            }\
        ";

VoidMediaItem::VoidMediaItem(const VoidImageSequence& media, QWidget* parent)
    : QFrame(parent)
    , m_Selected(false)
    , m_Playing(false)
    , m_Media(media)
{
    /* Build the layout */
    Build();

    /* Setup the UI */
    Setup();
}

VoidMediaItem::~VoidMediaItem()
{
}

void VoidMediaItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit doubleClicked(this);
}

void VoidMediaItem::focusInEvent(QFocusEvent* event)
{
    m_Selected = true;

    /* Set Stylesheet for Selected Item */
    setStyleSheet(m_SelectedSS.c_str());
}

void VoidMediaItem::focusOutEvent(QFocusEvent* event)
{
    m_Selected = false;

    /* Apply default stylesheet */
    setStyleSheet(m_DefaultSS.c_str());
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
    m_PlayLabel = new QLabel;

    m_Thumbnail = new QLabel;

    /* Add to the Layout */
    m_IntBtLayout->addWidget(m_RangeLabel);
    m_IntBtLayout->insertStretch(1, 1);
    m_IntBtLayout->addWidget(m_FramerateLabel);

    m_IntTpLayout->addWidget(m_NameLabel);
    m_IntTpLayout->insertStretch(1, 1);
    m_IntTpLayout->addWidget(m_TypeLabel);

    m_IconLayout->addWidget(m_PlayLabel);

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

    /* How the widget looks */
    setStyleSheet(m_DefaultSS.c_str());

    /* Fixed Height */
    setFixedHeight(60);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    /* Focus policy set to receive focus when clicked */
    setFocusPolicy(Qt::ClickFocus);
}

std::string VoidMediaItem::GetFramerate() const
{
    return Tools::to_trimmed_string(m_Media.Framerate()) + "fps";
}

std::string VoidMediaItem::GetRange() const
{
    return std::to_string(m_Media.FirstFrame()) + "-" + std::to_string(m_Media.LastFrame());
}

std::string VoidMediaItem::GetName() const
{
    return m_Media.Name();
}

std::string VoidMediaItem::GetType() const
{
    return m_Media.Type();
}

QPixmap VoidMediaItem::GetThumbnail() const
{   
    /* Resize the Frame to a thumbnail size */
    QImage scaled = QImage(m_Media.FirstImage().FullPath().c_str()).scaled(
            THUMBNAIL_SIZE, THUMBNAIL_SIZE, Qt::KeepAspectRatio
        );

    return QPixmap::fromImage(scaled);
}

void VoidMediaItem::SetPlaying(bool play)
{
    /* Update the play state */
    m_Playing = play;

    QStyle* s = style();

    if (m_Playing)
        m_PlayLabel->setPixmap(s->standardPixmap(
            s->SP_MediaPlay).scaled(
                ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio
            )
        );
    else
        m_PlayLabel->setPixmap(QPixmap(""));
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
