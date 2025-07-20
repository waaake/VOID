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
#include "VoidCore/Logging.h"
#include "VoidUi/VoidStyle.h"

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
    for (auto mit: m_MediaItems)
    {
        mit.second->deleteLater();
    }
}

QSize VoidMediaLister::sizeHint() const
{
    return QSize(300, 720);
}

// void VoidMediaLister::mousePressEvent(QMouseEvent* event)
// {
//     /* If the Media Lister is clicked by holding Alt key -> Clear any selected media */
//     if (event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier)
//         ClearSelection();
// }

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

// void VoidMediaLister::contextMenuEvent(QContextMenuEvent* event)
// {
//     /* Don't show up if nothing is selected */
//     if (m_CurrentSelected.empty())
//         return;

//     /* Create a context menu */
//     QMenu contextMenu(this);

//     /* Add the Defined actions */
//     contextMenu.addAction(m_PlayAction);
//     contextMenu.addAction(m_RemoveAction);

//     /* Show Menu */
//     contextMenu.exec(event->globalPos());
// }

// void VoidMediaLister::paintEvent(QPaintEvent* event)
// {
//     /* Standard draw for everything */
//     QWidget::paintEvent(event);

//     /* Create a Painter to draw the border */
//     QPainter painter(this);
//     painter.setPen(QPen(Qt::black, 2));
//     /* Draw the border with the painter */
//     painter.drawLine(0, 0, width(), 0);
// }

void VoidMediaLister::Build()
{
    /* Menu Actions */
    m_PlayAction = new QAction("Play Selected As Sequence");
    m_RemoveAction = new QAction("Remove Selected");

    /* Base */
    m_layout = new QVBoxLayout(this);

    /* Options {{{ */
    m_OptionsLayout = new QHBoxLayout;

    m_ViewButtonGroup = new QButtonGroup(this);
    m_ViewButtonGroup->setExclusive(true);

    /* View Toggle Buttons */
    m_ListViewToggle = new HighlightToggleButton(this);
    m_ListViewToggle->setIcon(QIcon(":resources/icons/icon_list_view.svg"));

    m_ThumbnailViewToggle = new HighlightToggleButton(this);
    m_ThumbnailViewToggle->setIcon(QIcon(":resources/icons/icon_grid_view.svg"));

    m_ViewButtonGroup->addButton(m_ListViewToggle, 0);
    m_ViewButtonGroup->addButton(m_ThumbnailViewToggle, 1);

    m_SortButton = new HighlightToggleButton(this);
    m_SortButton->setIcon(QIcon(":resources/icons/icon_sort_abc.svg"));
    m_SortButton->setFixedWidth(26);

    m_DeleteButton = new QPushButton;
    m_DeleteButton->setIcon(QIcon(":resources/icons/icon_delete.svg"));
    m_DeleteButton->setFixedWidth(26);

    m_SearchBar = new MediaSearchBar(this);

    m_OptionsLayout->addWidget(m_ListViewToggle);
    m_OptionsLayout->addWidget(m_ThumbnailViewToggle);
    m_OptionsLayout->addWidget(m_SearchBar);
    m_OptionsLayout->addWidget(m_SortButton);
    m_OptionsLayout->addWidget(m_DeleteButton);

    /* Setup margins */
    m_OptionsLayout->setContentsMargins(4, 0, 0, 0);
    /* }}} */

    /* Views {{{ */
    m_MediaView = new MediaView(this);
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
    // m_layout->addWidget(m_ScollArea);
    m_layout->addWidget(m_MediaView);

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

    /* Default View */
    m_ListViewToggle->setChecked(true);
}

void VoidMediaLister::Connect()
{
    /* Context Menu */
    connect(m_PlayAction, &QAction::triggered, this, &VoidMediaLister::AddSelectionToSequence);
    connect(m_RemoveAction, &QAction::triggered, this, &VoidMediaLister::RemoveSelectedMedia);

    /* Options */
    connect(m_DeleteButton, &QPushButton::clicked, this, &VoidMediaLister::RemoveSelectedMedia);
    connect(m_SearchBar, &MediaSearchBar::typed, m_MediaView, &MediaView::Search);
    connect(m_SortButton, &QPushButton::toggled, this, [this](const bool checked) { m_MediaView->EnableSorting(checked, Qt::AscendingOrder); });
    
    /* View Changed */
    #if _QT6_COMPAT
    connect(m_ViewButtonGroup, &QButtonGroup::idPressed, this, [this](int index) { m_MediaView->SetViewType(static_cast<MediaView::ViewType>(index)); });
    #else
    connect(m_ViewButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonPressed), this, [this](int index) { m_MediaView->SetViewType(static_cast<MediaView::ViewType>(index)); });
    #endif // _QT6_COMPAT

    /* Media Bridge */
    // connect(&MBridge::Instance(), &MBridge::mediaAdded, this, &VoidMediaLister::AddMedia);
    // connect(&MBridge::Instance(), &MBridge::mediaAboutToBeRemoved, this, &VoidMediaLister::RemoveMedia);

    /* List */
    connect(m_MediaView, &MediaView::itemDoubleClicked, this, &VoidMediaLister::IndexSelected);
    connect(m_MediaView, &MediaView::customContextMenuRequested, this, &VoidMediaLister::ShowContextMenu);
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
    /* Construct a Media Item from the provided media */
    VoidMediaItem* mediaItem = new VoidMediaItem(media);

    /* Connect signal */
    connect(mediaItem, &VoidMediaItem::selected, this, &VoidMediaLister::SelectItem);
    connect(mediaItem, &VoidMediaItem::doubleClicked, this, &VoidMediaLister::ChangeMedia);

    /* Add to the internal map holding all media items */
    m_MediaItems[media->Vuid()] = mediaItem;

    /* Add the Media Item to the Media Lister Scroll Widget */
    m_ScrollLayout->addWidget(mediaItem);

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

void VoidMediaLister::IndexSelected(const QModelIndex& index)
{
    if (!index.isValid())
        return;
 
    /* Emit the Media Clip dereferenced from the internal pointer */
    emit mediaChanged(*(static_cast<SharedMediaClip*>(index.internalPointer())));
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
    /* The currently selected indexes */
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();

    /* Nothing is selected */
    if (selected.empty())
        return;

    /* Vector to hold the underlying selected medias */
    std::vector<SharedMediaClip> m;

    /* Already aware of the amount of items which are to be copied */
    m.reserve(selected.size());

    for (const QModelIndex& index: selected)
    {
        /* Add the Media to the vector */
        m.emplace_back(*(static_cast<SharedMediaClip*>(index.internalPointer())));
    }

    /* Clear Existing Media which is marked as playing */
    // ClearPlaying();

    /* Copy the current selected item's clip to the vector */
    // for (VoidMediaItem* item: m_CurrentSelected)
    // {
    //     m.emplace_back(item->Clip());

    //     item->SetPlaying(true);
    //     m_CurrentPlaying.push_back(item);
    // }

    /* Emit that the sequence of playing media is now changed */
    emit playlistChanged(m);
}

void VoidMediaLister::RemoveMedia(const SharedMediaClip& media)
{
    /* Check if the media exists in the lister */
    auto it = m_MediaItems.find(media->Vuid());

    if (it != m_MediaItems.end())
    {
        /* Get the pointer to the Media item */
        VoidMediaItem* item = it->second;

        /* See if the item is present in selected array to remove that */
        m_CurrentPlaying.erase(std::remove(m_CurrentPlaying.begin(), m_CurrentPlaying.end(), item), m_CurrentPlaying.end());
        m_CurrentSelected.erase(std::remove(m_CurrentSelected.begin(), m_CurrentSelected.end(), item), m_CurrentSelected.end());
        
        /* Remove the Media item from the map */
        m_MediaItems.erase(it);

        /* Delete the media Item */
        item->setParent(nullptr);
        item->setVisible(false);

        /* Mark the item for deletion from Qt's memory */
        item->deleteLater();
        item = nullptr;
    }
}

void VoidMediaLister::ShowContextMenu(const Point& position)
{
    /* Show up only if we have selection */
    if (!m_MediaView->HasSelection())
        return;

    /* Create a context menu */
    QMenu contextMenu(this);

    /* Add the Defined actions */
    contextMenu.addAction(m_PlayAction);
    contextMenu.addAction(m_RemoveAction);

    /* Show Menu */
    contextMenu.exec(m_MediaView->mapToGlobal(position));
}

void VoidMediaLister::RemoveSelectedMedia()
{
    // for (VoidMediaItem* item: m_CurrentSelected)
    // {
    //     /* Remove from Playing if it exists */
    //     if (item->Playing())
    //     {
    //         m_CurrentPlaying.erase(
    //             std::remove(m_CurrentPlaying.begin(), m_CurrentPlaying.end(), item),
    //             m_CurrentPlaying.end()
    //         );
    //     }

    //     /* Retrieve the item's media clip before the item is deleted */
    //     SharedMediaClip c = item->Clip();

    //     /**
    //      * Remove the Item from the MediaBride for it to be removed across other components
    //      * that might be accessing it
    //      */
    //     // MBridge::Instance().RemoveClip(c);
    // }

    // /* Clear the Current Selection*/
    // m_CurrentSelected.clear();

    for (const QModelIndex& index : m_MediaView->SelectedIndexes())
    {
        /* Remove the Media at the given index */
        MBridge::Instance().Remove(index);
    }
}

VOID_NAMESPACE_CLOSE
