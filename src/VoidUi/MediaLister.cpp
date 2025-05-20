/* STD */
#include <filesystem>

/* Qt */
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

/* Internal */
#include "MediaLister.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

VoidMediaLister::VoidMediaLister(QWidget* parent)
{
    /* Build Layout */
    Build();

    /* Size Policy */
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    /* Accept drops */
    setAcceptDrops(true);
}

VoidMediaLister::~VoidMediaLister()
{
    /* Delete any media items */
    for (int i = 0; i < m_MediaItems.size(); i++)
    {
        m_MediaItems[0]->deleteLater();
    }
}

QSize VoidMediaLister::sizeHint() const
{
    return QSize(300, 720);
}

void VoidMediaLister::mousePressEvent(QMouseEvent* event)
{
    /* If the Media Lister is clicked by holding Alt key -> Clear any selected media */
    if (event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier)
        ClearSelection();
}

void VoidMediaLister::dragEnterEvent(QDragEnterEvent* event)
{
    /* Check if we have urls in the mime data */
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void VoidMediaLister::dropEvent(QDropEvent* event)
{
    /* Fetch all the urls which have been dropped */
    QList<QUrl> urls = event->mimeData()->urls();

    for (const QUrl& url : urls)
    {
        std::string path = url.toLocalFile().toStdString();

        /* Check if the path is a directory and emit the signal with the path if it is */
        if (std::filesystem::is_directory(path))
        {
            VOID_LOG_INFO("Dropped Media Directory: {0}", path);

            /* Emit the media dropped signal */
            emit mediaDropped(path);
        }
    }
}

void VoidMediaLister::Build()
{
    /* Base */
    m_layout = new QVBoxLayout(this);

    m_Scrollwidget = new QWidget;
    /* We want to make sure this widget tries to be the maximum of its contents */
    m_Scrollwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    /* Give this ScrollWidget a Vertical Layout */
    m_ScrollLayout = new QVBoxLayout(m_Scrollwidget);

    /* Spacing */
    m_ScrollLayout->setSpacing(2);
    m_ScrollLayout->setContentsMargins(4, 2, 4, 2);

    /* Create a Scroll area that will be incharge of scrolling its conents */
    m_ScollArea = new QScrollArea;
    /* Scroll area should be resizable as the UI grows or shrinks */
    m_ScollArea->setWidgetResizable(true);
    /* Use our Scoll container on this area */
    m_ScollArea->setWidget(m_Scrollwidget);
    /* Add to the base Layout */
    m_layout->addWidget(m_ScollArea);
}

void VoidMediaLister::ClearPlaying()
{
    if (m_CurrentPlaying)
    {
        m_CurrentPlaying->SetPlaying(false);
        /* Reset the current playing item */
        m_CurrentPlaying = nullptr;
    }
}

void VoidMediaLister::ClearSelection()
{
    if (m_CurrentSelectedItem)
    {
        m_CurrentSelectedItem->SetSelected(false);
        /* Reset the current selected item */
        m_CurrentSelectedItem = nullptr;
    }
}

void VoidMediaLister::AddMedia(const Media& media)
{
    /* Construct a Media Item from the provided media */
    VoidMediaItem* mediaItem = new VoidMediaItem(media);

    /* Connect signal */
    connect(mediaItem, &VoidMediaItem::clicked, this, &VoidMediaLister::SelectItem);
    connect(mediaItem, &VoidMediaItem::doubleClicked, this, &VoidMediaLister::ChangeMedia);

    /* Add to the internal array holding all media items */
    m_MediaItems.push_back(mediaItem);

    /* Add the Media Item to the Media Lister Scroll Widget */
    m_ScrollLayout->addWidget(mediaItem);

    /* Clear existing playing media */
    ClearPlaying();
    
    /* Set Playing state */
    m_CurrentPlaying = mediaItem;
    mediaItem->SetPlaying(true);

    /* Set Selection on this item */
    SelectItem(mediaItem);
}

void VoidMediaLister::SelectItem(VoidMediaItem* item)
{
    /* Don't have to do anything when the sender is the currently selected media */
    if (item == m_CurrentSelectedItem)
        return;
    
    /* Clear any existing selection */
    ClearSelection();

    /* Update the current selection */
    m_CurrentSelectedItem = item;
    item->SetSelected(true);
}

void VoidMediaLister::ChangeMedia(VoidMediaItem* item)
{
    /* 
     * Ignore if the currently playing item is the sender
     * Doesn't makes sense start playing the same item again and again when constantly clicking
     */
    if (item == m_CurrentPlaying)
        return;

    /* Clear the existing playing media */
    ClearPlaying();
    /* set the playing state for the media that has been double clicked on, i.e. changed to */
    m_CurrentPlaying = item;
    item->SetPlaying(true);

    /* Finally emit that the media has been changed */
    emit mediaChanged(item->Clip());
}

VOID_NAMESPACE_CLOSE
