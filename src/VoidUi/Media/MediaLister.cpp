// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Qt */
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenu>
#include <QMimeData>
#include <QUrl>

/* Internal */
#include "MediaLister.h"
#include "VoidCore/Logging.h"
#include "VoidUi/QExtensions/Tooltip.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

VoidMediaLister::VoidMediaLister(QWidget* parent)
    : QWidget(parent)
{
    /* Build Layout */
    Build();

    /* Connect Signals */
    Connect();

    /* Setup UI */
    Setup();

    /* Accept drops */
    setAcceptDrops(true);
}

VoidMediaLister::~VoidMediaLister()
{
    m_ProjectView->deleteLater();
    delete m_ProjectView;
    m_ProjectView = nullptr;

    m_MediaView->deleteLater();
    delete m_MediaView;
    m_MediaView = nullptr;

    m_PlayAction->deleteLater();
    delete m_PlayAction;
    m_PlayAction = nullptr;

    m_RemoveAction->deleteLater();
    delete m_RemoveAction;
    m_RemoveAction = nullptr;

    m_PlaylistMenu->deleteLater();
    delete m_PlaylistMenu;
    m_PlaylistMenu = nullptr;

    m_InspectMetadataAction->deleteLater();
    delete m_InspectMetadataAction;
    m_InspectMetadataAction = nullptr;
}

QSize VoidMediaLister::sizeHint() const
{
    return QSize(300, 720);
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
            MBridge::Instance().ImportDirectory(path, false);
        }
    }
}

void VoidMediaLister::Build()
{
    /* Menu Actions */
    m_PlayAction = new QAction("Play Selected As Sequence");
    m_RemoveAction = new QAction("Remove Selected");
    m_InspectMetadataAction = new QAction("Show in Metadata Viewer");
    
    m_PlaylistMenu = new QMenu("Add to Playlist");
    /* Add any playlists which are present in the active project */
    RebuildPlaylistMenu();

    /* Shortcuts */
#ifdef __APPLE__
    m_DeleteShortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
#else
    m_DeleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
#endif

    /* Base */
    m_layout = new QVBoxLayout(this);

    /* Options {{{ */
    m_OptionsLayout = new QHBoxLayout;

    m_ViewButtonGroup = new QButtonGroup(this);
    m_ViewButtonGroup->setExclusive(true);

    /* View Toggle Buttons */
    m_ListViewToggle = new HighlightToggleButton(this);
    m_ListViewToggle->setIcon(QIcon(":resources/icons/icon_list_view.svg"));
    m_ListViewToggle->setToolTip(
        ToolTipString(
            "Detailed List View",
            "Shows items in a Vertical List with details."
        ).c_str()
    );

    m_ThumbnailViewToggle = new HighlightToggleButton(this);
    m_ThumbnailViewToggle->setIcon(QIcon(":resources/icons/icon_grid_view.svg"));
    m_ThumbnailViewToggle->setToolTip(
        ToolTipString(
            "Thumbnail View",
            "Shows items in a dynamic Thumbnail Grid."
        ).c_str()
    );

    m_ViewButtonGroup->addButton(m_ListViewToggle, 0);
    m_ViewButtonGroup->addButton(m_ThumbnailViewToggle, 1);

    m_SortButton = new HighlightToggleButton(this);
    m_SortButton->setIcon(QIcon(":resources/icons/icon_sort_abc.svg"));
    m_SortButton->setFixedWidth(26);
    m_SortButton->setToolTip(
        ToolTipString(
            "Sort Media",
            "Sorts Media in an Alphebetical order based on the name."
        ).c_str()
    );

    m_SearchBar = new MediaSearchBar(this);
    m_SearchBar->setToolTip(
        ToolTipString(
            "Search Media",
            "Searches and filters media in the Media View as per the provided Text."
        ).c_str()
    );

    m_OptionsLayout->addWidget(m_ListViewToggle);
    m_OptionsLayout->addWidget(m_ThumbnailViewToggle);
    m_OptionsLayout->addWidget(m_SearchBar);
    m_OptionsLayout->addWidget(m_SortButton);

    /* Setup margins */
    m_OptionsLayout->setContentsMargins(4, 0, 4, 0);
    /* }}} */

    /* Views {{{ */
    m_ViewSplitter = new QSplitter(Qt::Horizontal);

    m_ProjectView = new ProjectView(this);
    m_MediaView = new MediaView(this);

    m_ViewSplitter->addWidget(m_ProjectView);
    m_ViewSplitter->addWidget(m_MediaView);
    /* }}} */

    /* Add to the base Layout */
    m_layout->addLayout(m_OptionsLayout);
    m_layout->addWidget(m_ViewSplitter);

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
    p.setColor(QPalette::Window, palette().color(QPalette::Dark));

    this->setPalette(p);

    /* Splitter sizes */
    int w = sizeHint().width();
    int p_width = static_cast<int>(w * 0.3);
    /* Project view is smaller than media view*/
    m_ViewSplitter->setSizes({p_width, w - p_width});

    /* Load Settings from Preferences */
    SetFromPreferences();
}

void VoidMediaLister::Connect()
{
    /* Context Menu */
    connect(m_PlayAction, &QAction::triggered, this, &VoidMediaLister::AddSelectionToSequence);
    connect(m_RemoveAction, &QAction::triggered, this, &VoidMediaLister::RemoveSelectedMedia);
    connect(m_InspectMetadataAction, &QAction::triggered, this, &VoidMediaLister::InspectMetadata);

    /* Options */
    connect(m_SearchBar, &MediaSearchBar::typed, m_MediaView, &MediaView::Search);
    connect(m_SortButton, &QPushButton::toggled, this, [this](const bool checked) { m_MediaView->EnableSorting(checked, Qt::AscendingOrder); });
    
    /* View Changed */
    /* The call to buttonToggled is a slightly expensive as this gets called 2 times if we have n buttons (once for checked off and once for checked on) */
    connect(m_ViewButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton*, bool)>(&QButtonGroup::buttonToggled), this, [this](QAbstractButton* b, bool s)
    { 
        if (s)
            m_MediaView->SetViewType(static_cast<MediaView::ViewType>(m_ViewButtonGroup->id(b)));
    });

    /* List */
    connect(m_MediaView, &MediaView::itemDoubleClicked, this, &VoidMediaLister::IndexSelected);
    connect(m_MediaView, &MediaView::customContextMenuRequested, this, &VoidMediaLister::ShowContextMenu);

    connect(m_ProjectView, &ProjectView::itemClicked, this, [this](const QModelIndex& index)
    {
        MBridge::Instance().SetCurrentProject(index);
        RebuildPlaylistMenu();
    });

    /* Shortcut */
    connect(m_DeleteShortcut, &QShortcut::activated, this, &VoidMediaLister::RemoveSelectedMedia);

    /* Preferences */
    connect(&VoidPreferences::Instance(), &VoidPreferences::updated, this, &VoidMediaLister::SetFromPreferences);
}

void VoidMediaLister::IndexSelected(const QModelIndex& index)
{
    if (!index.isValid())
        return;
 
    /* Emit the Media Clip dereferenced from the internal pointer */
    emit mediaChanged(*(static_cast<SharedMediaClip*>(index.internalPointer())));
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

    for (const QModelIndex& index : selected)
    {
        /* Add the Media to the vector */
        m.emplace_back(*(static_cast<SharedMediaClip*>(index.internalPointer())));
    }

    /* Emit that the sequence of playing media is now changed */
    emit playlistChanged(m);
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

    contextMenu.addSeparator();
    contextMenu.addAction(m_InspectMetadataAction);

    contextMenu.addSeparator();
    contextMenu.addMenu(m_PlaylistMenu);

    /* Show Menu */
    #if _QT6
    /**
     * Qt6 mapToGlobal returns QPointF while menu.exec expects QPoint
     */
    contextMenu.exec(m_MediaView->mapToGlobal(position).toPoint());
    #else
    contextMenu.exec(m_MediaView->mapToGlobal(position));
    #endif // _QT6
}

void VoidMediaLister::RemoveSelectedMedia()
{
    /* Push all of the selected indexes for removal */
    MBridge::Instance().RemoveMedia(m_MediaView->SelectedIndexes());
}

void VoidMediaLister::InspectMetadata()
{
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();
    if (selected.empty())
        return;

    /* We can only inspect one item at a time */
    emit metadataInspected(*(static_cast<SharedMediaClip*>(selected[0].internalPointer())));
}

void VoidMediaLister::SetFromPreferences()
{
    /* Default View */
    if (!VoidPreferences::Instance().GetMediaViewType())
        m_ListViewToggle->setChecked(true);
    else
        m_ThumbnailViewToggle->setChecked(true);
}

void VoidMediaLister::RebuildPlaylistMenu()
{
    m_PlaylistMenu->clear();

    m_CreatePlaylistAction = new QAction("Create Playlist...", m_PlaylistMenu);
    m_PlaylistMenu->addAction(m_CreatePlaylistAction);

    for (Playlist* playlist : *MBridge::Instance().ActiveProject()->PlaylistMediaModel())
    {
        QAction* action = new QAction(playlist->Name().c_str(), m_PlaylistMenu);
        connect(action, &QAction::triggered, this, [=]() { AddSelectionToPlaylist(playlist); });
        m_PlaylistMenu->addAction(action);
    }
}

void VoidMediaLister::AddSelectionToPlaylist(Playlist* playlist)
{
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();

    /* Nothing is selected */
    if (selected.empty())
        return;

    for (const QModelIndex& index : selected)
    {
        /* Add the Media to the Playlist */
        playlist->AddMedia(*(static_cast<SharedMediaClip*>(index.internalPointer())));
    }
}

VOID_NAMESPACE_CLOSE
