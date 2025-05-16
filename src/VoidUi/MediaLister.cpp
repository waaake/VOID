/* Qt */
#include <QMouseEvent>

/* Internal */
#include "MediaLister.h"

VOID_NAMESPACE_OPEN

VoidMediaLister::VoidMediaLister(QWidget* parent)
{
    /* Build Layout */
    Build();

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
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

void VoidMediaLister::Build()
{
    /* Base */
    m_layout = new QVBoxLayout(this);

    m_Scrollwidget = new QWidget;
    /* We want to make sure this widget tries to be the maximum of its contents */
    m_Scrollwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    /* Give this ScrollWidget a Vertical Layout */
    m_ScrollLayout = new QVBoxLayout(m_Scrollwidget);

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

void VoidMediaLister::AddMedia(const VoidImageSequence& media)
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
    emit mediaChanged(item->GetMedia());
}

VOID_NAMESPACE_CLOSE
