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
#include "TagWidget.h"
#include "VoidCore/Logging.h"
#include "VoidIconForge/IconForge.h"
#include "VoidObjects/Preferences/Preferences.h"
#include "VoidQExtensions/Tooltip.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"
#include "VoidMediaPlayer/Project/ProjectBridge.h"
#include "VoidMediaPlayer/Player/PlayerBridge.h"

VOID_NAMESPACE_OPEN

#define _ENTITY_TYPE(x) static_cast<ProjectEntity::Type>(x.data(static_cast<int>(EntityModel::MRoles::Type)).toInt())

VoidMediaLister::VoidMediaLister(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Connect();
    Setup();

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

    m_PlayMenu->deleteLater();
    delete m_PlayMenu;
    m_PlayMenu = nullptr;

    m_TagsMenu->deleteLater();
    delete m_TagsMenu;
    m_TagsMenu = nullptr;

    m_SequenceMenu->deleteLater();
    delete m_SequenceMenu;
    m_SequenceMenu = nullptr;

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

void VoidMediaLister::dragEnterEvent(QDragEnterEvent* event)
{
    /* Check if we have urls in the mime data */
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void VoidMediaLister::dropEvent(QDropEvent* event)
{
    /* Fetch all the urls which have been dropped */
    QList<QUrl> urls = event->mimeData()->urls();
    std::vector<std::string> directories;
    directories.reserve(urls.size());

    for (const QUrl& url : urls)
    {
        std::string path = url.toLocalFile().toStdString();
        if (std::filesystem::is_directory(path))
            directories.push_back(path);
    }

    if (!directories.empty())
        _MediaBridge.ImportDirectory(directories, true);
}

void VoidMediaLister::Build()
{
    m_PlayMenu = new QMenu("Play");
    m_PlayAction = new QAction("Play Selected As Sequence", m_PlayMenu);
    m_PlayAsListAction = new QAction("Play Selected in Queue", m_PlayMenu);
    m_AddToQueueAction = new QAction("Add Selected to Queue", m_PlayMenu);
    m_PlayAsGridAction = new QAction("Play Selected as Grid", m_PlayMenu);
    m_PlayMenu->addAction(m_PlayAction);
    m_PlayMenu->addAction(m_PlayAsListAction);
    m_PlayMenu->addAction(m_AddToQueueAction);
    m_PlayMenu->addAction(m_PlayAsGridAction);

    m_RemoveAction = new QAction("Remove Selected");
    m_InspectMetadataAction = new QAction("Show in Metadata Viewer");

    m_TagsMenu = new QMenu("Tags");
    m_AddTagAction = new QAction("Add tag...", m_TagsMenu);
    m_ClearTagsAction = new QAction("Clear all tags", m_TagsMenu);
    m_TagsMenu->addAction(m_AddTagAction);
    m_TagsMenu->addAction(m_ClearTagsAction);

    m_PlaylistMenu = new QMenu("Add to Playlist");
    // Add any playlists which are present in the active project
    RebuildPlaylistMenu();

    m_SequenceMenu = new QMenu("Sequence");
    m_RenameAction = new QAction("Rename", m_SequenceMenu);
    m_AddSequenceAction = new QAction("Add Sequence", m_SequenceMenu);
    m_SequenceMenu->addAction(m_AddSequenceAction);
    m_SequenceMenu->addAction(m_RenameAction);

    m_PrimaryViewShortcut = new QShortcut(QKeySequence(Qt::Key_1), this);
    m_PrimaryViewShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    m_SecondaryViewShortcut = new QShortcut(QKeySequence(Qt::Key_2), this);
    m_SecondaryViewShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    m_layout = new QVBoxLayout(this);

    m_OptionsLayout = new QHBoxLayout;
    m_ViewButtonGroup = new QButtonGroup(this);
    m_ViewButtonGroup->setExclusive(true);

    // View Toggle Buttons
    m_DetailedListViewToggle = new HighlightToggleButton(this);
    m_DetailedListViewToggle->setIcon(IconForge::GetIcon(IconType::icon_view_stream, _DARK_COLOR(QPalette::Text, 100)));
    m_DetailedListViewToggle->setToolTip(
        ToolTipString(
            "Detailed List View",
            "Shows items in a Vertical List with details."
        ).c_str()
    );

    m_ListViewToggle = new HighlightToggleButton(this);
    m_ListViewToggle->setIcon(IconForge::GetIcon(IconType::icon_lists, _DARK_COLOR(QPalette::Text, 100)));
    m_ListViewToggle->setToolTip(
        ToolTipString(
            "List View",
            "Shows items in a Vertical List."
        ).c_str()
    );

    m_ThumbnailViewToggle = new HighlightToggleButton(this);
    m_ThumbnailViewToggle->setIcon(IconForge::GetIcon(IconType::icon_grid_view, _DARK_COLOR(QPalette::Text, 100)));
    m_ThumbnailViewToggle->setToolTip(
        ToolTipString(
            "Thumbnail View",
            "Shows items in a dynamic Thumbnail Grid."
        ).c_str()
    );

    m_ViewButtonGroup->addButton(m_DetailedListViewToggle, 0);
    m_ViewButtonGroup->addButton(m_ListViewToggle, 1);
    m_ViewButtonGroup->addButton(m_ThumbnailViewToggle, 2);

    m_SortButton = new HighlightToggleButton(this);
    m_SortButton->setIcon(IconForge::GetIcon(IconType::icon_sort_by_alpha, _DARK_COLOR(QPalette::Text, 100)));
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

    m_ScaleSlider = new QSlider(Qt::Horizontal, this);

    m_OptionsLayout->addWidget(m_DetailedListViewToggle);
    m_OptionsLayout->addWidget(m_ListViewToggle);
    m_OptionsLayout->addWidget(m_ThumbnailViewToggle);
    m_OptionsLayout->addWidget(m_SearchBar);
    m_OptionsLayout->addWidget(m_ScaleSlider);
    m_OptionsLayout->addWidget(m_SortButton);

    m_OptionsLayout->setContentsMargins(4, 0, 4, 0);

    m_ViewSplitter = new QSplitter(Qt::Horizontal);
    m_ProjectView = new ProjectView(this);
    m_MediaView = new MediaView(this);
    m_ViewSplitter->addWidget(m_ProjectView);
    m_ViewSplitter->addWidget(m_MediaView);

    m_layout->addLayout(m_OptionsLayout);
    m_layout->addWidget(m_ViewSplitter);

    int left, top, right, bottom;
    m_layout->getContentsMargins(&left, &top, &right, &bottom);
    m_layout->setContentsMargins(0, top, 0, 2);
}

void VoidMediaLister::Setup()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    /* Dark Panel */
    QPalette p = this->palette();
    p.setColor(QPalette::Window, palette().color(QPalette::Dark));
    setPalette(p);

    m_ScaleSlider->setRange(100, 150);

    int w = sizeHint().width();
    int p_width = static_cast<int>(w * 0.3);
    m_ViewSplitter->setSizes({p_width, w - p_width});

    // Load Settings from Preferences
    SetFromPreferences();
}

void VoidMediaLister::Connect()
{
    // Context Menu
    connect(m_AddSequenceAction, &QAction::triggered, this, []() -> void 
    {
        _ProjectBridge.AddSequence(_MediaBridge.ActiveProject());
    });
    connect(m_PlayAction, &QAction::triggered, this, &VoidMediaLister::AddSelectionToSequence);
    connect(m_PlayAsListAction, &QAction::triggered, this, &VoidMediaLister::PlaySelectionAsQueue);
    connect(m_AddToQueueAction, &QAction::triggered, this, &VoidMediaLister::AddSelectionToQueue);
    connect(m_PlayAsGridAction, &QAction::triggered, this, &VoidMediaLister::PlaySelectionAsGrid);
    connect(m_RemoveAction, &QAction::triggered, m_MediaView, &MediaView::RemoveSelectedMedia);
    connect(m_InspectMetadataAction, &QAction::triggered, this, &VoidMediaLister::InspectMetadata);
    connect(m_AddTagAction, &QAction::triggered, this, &VoidMediaLister::AddTagToSelected);
    connect(m_ClearTagsAction, &QAction::triggered, this, &VoidMediaLister::ClearTagsFromSelected);
    connect(m_RenameAction, &QAction::triggered, this, [this]() -> void { m_MediaView->edit(m_MediaView->currentIndex()); });

    connect(m_SearchBar, &MediaSearchBar::typed, m_MediaView, &MediaView::Search);
    connect(m_SortButton, &QPushButton::toggled, this, [this](const bool checked) { m_MediaView->EnableSorting(checked, Qt::AscendingOrder); });
    connect(m_ScaleSlider, &QSlider::valueChanged, this, [this](int value) { RescaleThumbnails((float)value / 100); });

    /* The call to buttonToggled is a slightly expensive as this gets called 2 times if we have n buttons (once for checked off and once for checked on) */
    connect(m_ViewButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton*, bool)>(&QButtonGroup::buttonToggled), this, [this](QAbstractButton* b, bool s)
    {
        if (s)
        {
            auto viewType = static_cast<MediaView::ViewType>(m_ViewButtonGroup->id(b));
            m_MediaView->SetViewType(viewType);
            m_ScaleSlider->setVisible(viewType == MediaView::ViewType::ThumbnailView);
        }
    });

    /* List */
    connect(m_MediaView, &MediaView::itemDoubleClicked, this, &VoidMediaLister::IndexSelected);
    connect(m_MediaView, &MediaView::customContextMenuRequested, this, &VoidMediaLister::ShowContextMenu);
    connect(m_MediaView, &MediaView::tagClicked, this, &VoidMediaLister::EditSelectedTags);

    connect(m_ProjectView, &ProjectView::itemClicked, this, [this](const QModelIndex& index)
    {
        _MediaBridge.SetCurrentProject(index);
        RebuildPlaylistMenu();
    });
    connect(&_MediaBridge, &MBridge::playlistCreated, this, &VoidMediaLister::RebuildPlaylistMenu);
    connect(&_MediaBridge, &MBridge::updated, this, &VoidMediaLister::RebuildPlaylistMenu);

    /* Shortcut */
    connect(m_PrimaryViewShortcut, &QShortcut::activated, this, &VoidMediaLister::AddToPrimaryViewer);
    connect(m_SecondaryViewShortcut, &QShortcut::activated, this, &VoidMediaLister::AddToSecondaryViewer);

    /* Preferences */
    connect(&VoidPreferences::Instance(), &VoidPreferences::updated, this, &VoidMediaLister::SetFromPreferences);
}

void VoidMediaLister::IndexSelected(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    if (_ENTITY_TYPE(index) == ProjectEntity::Type::MEDIA)
        _PlayerBridge.SetMedia(*(static_cast<SharedMediaClip*>(index.internalPointer())));
    else
        _PlayerBridge.SetSequence(*(static_cast<SharedPlaybackSequence*>(index.internalPointer())));
}

void VoidMediaLister::AddSelectionToSequence()
{
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();

    if (selected.empty())
        return;

    std::vector<SharedMediaClip> m;
    m.reserve(selected.size());
    for (const QModelIndex& index : selected)
    {
        if (_ENTITY_TYPE(index) == ProjectEntity::Type::MEDIA)
            m.emplace_back(*(static_cast<SharedMediaClip*>(index.internalPointer())));
    }

    _PlayerBridge.SetMedia(m);
}

void VoidMediaLister::PlaySelectionAsQueue()
{
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();

    if (selected.empty())
        return;

    std::vector<SharedMediaClip> m;
    m.reserve(selected.size());

    for (const QModelIndex& index: selected)
    {
        if (_ENTITY_TYPE(index) == ProjectEntity::Type::MEDIA)
            m.emplace_back(*(static_cast<SharedMediaClip*>(index.internalPointer())));
    }

    _PlayerBridge.ClearQueue();
    _PlayerBridge.AddToQueue(m);
}

void VoidMediaLister::AddSelectionToQueue()
{
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();

    if (selected.empty())
        return;

    std::vector<SharedMediaClip> m;
    m.reserve(selected.size());

    for (const QModelIndex& index: selected)
    {
        if (_ENTITY_TYPE(index) == ProjectEntity::Type::MEDIA)
            m.emplace_back(*(static_cast<SharedMediaClip*>(index.internalPointer())));
    }

    _PlayerBridge.AddToQueue(m, false);
}

void VoidMediaLister::PlaySelectionAsGrid()
{
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();

    if (selected.empty())
        return;

    std::vector<SharedMediaClip> m;
    m.reserve(selected.size());

    for (const QModelIndex& index: selected)
    {
        if (_ENTITY_TYPE(index) == ProjectEntity::Type::MEDIA)
            m.emplace_back(*(static_cast<SharedMediaClip*>(index.internalPointer())));
    }

    _PlayerBridge.SetGrid(m);
}

void VoidMediaLister::ShowContextMenu(const _QPoint& position)
{
    QMenu contextMenu(this);

    const bool selection = m_MediaView->HasSelection();
    contextMenu.addMenu(m_PlayMenu);
    m_PlayMenu->setEnabled(selection);

    contextMenu.addMenu(m_TagsMenu);
    m_TagsMenu->setEnabled(selection);

    contextMenu.addAction(m_InspectMetadataAction);
    contextMenu.addAction(m_RemoveAction);
    
    m_InspectMetadataAction->setEnabled(selection);
    m_RemoveAction->setEnabled(selection);

    contextMenu.addSeparator();
    contextMenu.addMenu(m_SequenceMenu);
    m_RenameAction->setEnabled(CanRenameSelection());

    contextMenu.addMenu(m_PlaylistMenu);
    m_PlaylistMenu->setEnabled(selection);

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

void VoidMediaLister::RescaleThumbnails(float scale)
{
    m_MediaView->SetThumbnailScale(scale);
}

// void VoidMediaLister::RemoveSelectedMedia()
// {
//     /* Push all of the selected indexes for removal */
//     _MediaBridge.RemoveMedia(m_MediaView->SelectedIndexes());
// }

void VoidMediaLister::InspectMetadata()
{
    QModelIndex current = m_MediaView->currentIndex();
    if (_ENTITY_TYPE(current) == ProjectEntity::Type::MEDIA)
        emit metadataInspected(_MediaBridge.MediaAt(current));
}

void VoidMediaLister::EditEffects()
{
    QModelIndex current = m_MediaView->currentIndex();
    if (current.isValid() && _ENTITY_TYPE(current) == ProjectEntity::Type::MEDIA)
    {
        emit effectsEdited(_MediaBridge.MediaAt(current));
    }
}

void VoidMediaLister::SetFromPreferences()
{
    switch(static_cast<MediaView::ViewType>(VoidPreferences::Instance().GetMediaViewType()))
    {
        case MediaView::ViewType::ListView:
            m_ListViewToggle->setChecked(true);
            break;
        case MediaView::ViewType::ThumbnailView:
            m_ThumbnailViewToggle->setChecked(true);
            break;
        case MediaView::ViewType::DetailedListView:
        default:
            m_DetailedListViewToggle->setChecked(true);
    }
}

void VoidMediaLister::RebuildPlaylistMenu()
{
    m_PlaylistMenu->clear();

    m_CreatePlaylistAction = new QAction("Create Playlist...", m_PlaylistMenu);
    connect(m_CreatePlaylistAction, &QAction::triggered, this, [this]() { AddSelectionToPlaylist(_MediaBridge.NewPlaylist()); });
    m_PlaylistMenu->addAction(m_CreatePlaylistAction);

    for (Playlist* playlist : *_MediaBridge.ActiveProject()->PlaylistMediaModel())
    {
        QAction* action = new QAction(playlist->Name().c_str(), m_PlaylistMenu);
        connect(action, &QAction::triggered, this, [=]() { AddSelectionToPlaylist(playlist); });
        m_PlaylistMenu->addAction(action);
    }
}

void VoidMediaLister::AddToPrimaryViewer() const
{
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();
    if (selected.empty())
        return;

    _PlayerBridge.SetMedia(_MediaBridge.MediaAt(selected[0]), PlayerViewBuffer::A);
}

void VoidMediaLister::AddToSecondaryViewer() const
{
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();
    if (selected.empty())
        return;

    _PlayerBridge.SetMedia(_MediaBridge.MediaAt(selected[0]), PlayerViewBuffer::B);
}

void VoidMediaLister::AddSelectionToPlaylist(Playlist* playlist) const
{
    if (!playlist)
        return;

    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();

    /* Nothing is selected */
    if (selected.empty())
        return;

    _MediaBridge.AddToPlaylist(selected, playlist);
}

void VoidMediaLister::AddTagToSelected()
{
    QModelIndex current = m_MediaView->currentIndex();
    if (current.isValid())
    {
        TagWidget t(current, this);
        t.MoveTo(m_MediaView->mapToGlobal(m_MediaView->visualRect(current).center()));
        t.exec();
    }
}

void VoidMediaLister::EditSelectedTags(const QModelIndex& index, const QPoint& position)
{
    SharedMediaClip clip = _MediaBridge.MediaAt(index);
    if (clip->HasTags())
    {
        TagEditor t(clip, index, this);
        t.MoveTo(m_MediaView->mapToGlobal(position));
        t.exec();
    }
}

void VoidMediaLister::ClearTagsFromSelected()
{
    QModelIndex current = m_MediaView->currentIndex();
    if (current.isValid())
    {
        SharedMediaClip clip = _MediaBridge.MediaAt(current);
        _MediaBridge.ClearTags(clip);
    }
}

bool VoidMediaLister::CanRenameSelection() const
{
    QItemSelectionModel* selection = m_MediaView->selectionModel();
    if (selection->selectedRows().size() != 1)
        return false;
    
    return _ENTITY_TYPE(m_MediaView->currentIndex()) == ProjectEntity::Type::SEQUENCE;
}

VOID_NAMESPACE_CLOSE
