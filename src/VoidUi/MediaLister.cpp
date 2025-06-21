/* STD */
#include <filesystem>

/* Qt */
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QUrl>

/* Internal */
#include "MediaLister.h"
#include "VoidStyle.h"
#include "VoidCore/Logging.h"
#include "PlayerWindow.h"

VOID_NAMESPACE_OPEN

VoidMediaLister::VoidMediaLister(QWidget* parent)
    : QWidget(parent)
{
    /* Build Layout */
    Build();

    /* Setup UI */
    Setup();

    /* Connect Signals */
    Connect();

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

void VoidMediaLister::contextMenuEvent(QContextMenuEvent* event)
{
    /* Don't show up if nothing is selected */
    if (m_CurrentSelected.empty())
        return;

    /* Create a context menu */
    QMenu contextMenu(this);

    /* Add the Defined actions */
    contextMenu.addAction(m_PlayAction);
    contextMenu.addAction(m_RemoveAction);

    /* Show Menu */
    contextMenu.exec(event->globalPos());
}

void VoidMediaLister::paintEvent(QPaintEvent* event)
{
    /* Standard draw for everything */
    QWidget::paintEvent(event);

    /* Create a Painter to draw the border */
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 2));
    /* Draw the border with the painter */
    painter.drawLine(0, 0, width(), 0);
}

void VoidMediaLister::Build()
{
    /* Menu Actions */
    m_PlayAction = new QAction("Play Selected As Sequence");
    m_RemoveAction = new QAction("Remove Selected");

    /* Base */
    m_layout = new QVBoxLayout(this);

    /* Options {{{ */
    m_OptionsLayout = new QHBoxLayout;
    m_DeleteButton = new QPushButton;
    m_DeleteButton->setIcon(QIcon(":resources/icons/icon_delete.svg"));
    m_DeleteButton->setFixedWidth(26);

    m_OptionsLayout->addStretch(1);
    m_OptionsLayout->addWidget(m_DeleteButton);
    /* }}} */


    m_Scrollwidget = new QWidget;
    /* We want to make sure this widget tries to be the maximum of its contents */
    m_Scrollwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    /* Give this ScrollWidget a Vertical Layout */
    m_ScrollLayout = new QVBoxLayout(m_Scrollwidget);

    /* Spacing */
    m_ScrollLayout->setSpacing(2);
    m_ScrollLayout->setContentsMargins(2, 2, 2, 2);

    /* Create a Scroll area that will be incharge of scrolling its conents */
    m_ScollArea = new QScrollArea;
    /* Scroll area should be resizable as the UI grows or shrinks */
    m_ScollArea->setWidgetResizable(true);
    /* Use our Scoll container on this area */
    m_ScollArea->setWidget(m_Scrollwidget);

    /* Add to the base Layout */
    m_layout->addLayout(m_OptionsLayout);
    m_layout->addWidget(m_ScollArea);

    /* Spacing */
    int left, top, right, bottom;
    m_layout->getContentsMargins(&left, &top, &right, &bottom);
    /* Only adjust the right side spacing to make it cleaner against the viewer */
    m_layout->setContentsMargins(0, top, 0, 2);
}

void VoidMediaLister::Setup()
{
    /* Size Policy */
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    /* Dark Panel */
    QPalette p = this->palette();
    p.setColor(QPalette::Window, VOID_SEMI_DARK_COLOR);

    this->setPalette(p);
}

void VoidMediaLister::Connect()
{
    /* Context Menu */
    connect(m_PlayAction, &QAction::triggered, this, &VoidMediaLister::AddSelectionToSequence);
    connect(m_RemoveAction, &QAction::triggered, this, &VoidMediaLister::RemoveSelectedMedia);

    /* Options */
    connect(m_DeleteButton, &QPushButton::clicked, this, &VoidMediaLister::RemoveSelectedMedia);
}

void VoidMediaLister::ClearPlaying()
{
    for (VoidMediaItem* item: m_CurrentPlaying)
    {
        item->SetPlaying(false);
    }

    /* Clear only the vector */
    /* The Media Item pointer should not get dereferenced at the moment */
    m_CurrentPlaying.clear();
}

void VoidMediaLister::ClearSelection()
{
    for (VoidMediaItem* item: m_CurrentSelected)
    {
        item->SetSelected(false);
    }

    /* Clear only the vector */
    /* The Media Item pointer should not get dereferenced at the moment */
    m_CurrentSelected.clear();
}

void VoidMediaLister::AddMedia(const SharedMediaClip& media)
{
    /* Clear existing playing media */
    ClearPlaying();

    /* Construct a Media Item from the provided media */
    VoidMediaItem* mediaItem = new VoidMediaItem(media);

    /* Connect signal */
    connect(mediaItem, &VoidMediaItem::selected, this, &VoidMediaLister::SelectItem);
    connect(mediaItem, &VoidMediaItem::doubleClicked, this, &VoidMediaLister::ChangeMedia);

    /* Add to the internal array holding all media items */
    m_MediaItems.push_back(mediaItem);

    /* Add the Media Item to the Media Lister Scroll Widget */
    m_ScrollLayout->addWidget(mediaItem);

    /* Set Playing state */
    m_CurrentPlaying.push_back(mediaItem);
    mediaItem->SetPlaying(true);

    /* Set Selection on this item */
    SelectItem(mediaItem);
}

void VoidMediaLister::SelectItem(VoidMediaItem* item, bool clear)
{
    /* Don't have to do anything when the sender is the currently selected media */
    // if (item == m_CurrentSelectedItem)
    //     return;

    /* Clear any existing selection if defined so */
    if (clear)
        ClearSelection();

    /* Update the current selection */
    m_CurrentSelected.push_back(item);
    item->SetSelected(true);
}

void VoidMediaLister::ChangeMedia(VoidMediaItem* item)
{
    /*
     * Ignore if the currently playing item is the sender
     * Doesn't makes sense start playing the same item again and again when constantly clicking
     */
    // if (item == m_CurrentPlaying)
    //     return;

    /* Clear the existing playing media */
    ClearPlaying();
    /* set the playing state for the media that has been double clicked on, i.e. changed to */
    m_CurrentPlaying.push_back(item);
    item->SetPlaying(true);

    /* Finally emit that the media has been changed */
    emit mediaChanged(item->Clip());
}

void VoidMediaLister::AddSelectionToSequence()
{
    std::vector<SharedMediaClip> m;
    /* Already aware of the amount of items which are to be copied */
    m.reserve(m_CurrentSelected.size());

    /* Clear Existing Media which is marked as playing */
    ClearPlaying();

    /* Copy the current selected item's clip to the vector */
    for (VoidMediaItem* item: m_CurrentSelected)
    {
        m.emplace_back(item->Clip());

        item->SetPlaying(true);
        m_CurrentPlaying.push_back(item);
    }

    /* Emit that the sequence of playing media is now changed */
    emit playlistChanged(m);
}

void VoidMediaLister::RemoveSelectedMedia()
{
    for (VoidMediaItem* item: m_CurrentSelected)
    {
        /* Remove item from the media list as well */
        m_MediaItems.erase(
            std::remove(m_MediaItems.begin(), m_MediaItems.end(), item),
            m_MediaItems.end()
        );

        /* Remove from Playing if it exists */
        if (item->Playing())
        {
            m_CurrentPlaying.erase(
                std::remove(m_CurrentPlaying.begin(), m_CurrentPlaying.end(), item),
                m_CurrentPlaying.end()
            );
        }

        /* Prepare for the item to be deleted */
        item->setParent(nullptr);
        item->setVisible(false);

        /* Mark the item for deletion from Qt's memory */
        item->deleteLater();
        item = nullptr;
    }

    /* Clear the Current Selection*/
    m_CurrentSelected.clear();
}

VOID_NAMESPACE_CLOSE
