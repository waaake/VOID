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
#include "PlayLister.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Engine/IconForge.h"
#include "VoidUi/QExtensions/Tooltip.h"
#include "VoidUi/Player/PlayerBridge.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

VoidPlayLister::VoidPlayLister(QWidget* parent)
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

VoidPlayLister::~VoidPlayLister()
{
    m_MediaView->deleteLater();
    delete m_MediaView;
    m_MediaView = nullptr;
}

QSize VoidPlayLister::sizeHint() const
{
    return QSize(300, 720);
}

void VoidPlayLister::dragEnterEvent(QDragEnterEvent* event)
{
    /* Check if we have urls in the mime data */
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void VoidPlayLister::dropEvent(QDropEvent* event)
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
            // _MediaBridge.ImportDirectory(path, false);
        }
    }
}

void VoidPlayLister::Build()
{
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
    m_ListViewToggle->setIcon(IconForge::GetIcon(IconType::icon_lists, _DARK_COLOR(QPalette::Text, 150)));
    m_ListViewToggle->setToolTip(
        ToolTipString(
            "List View",
            "Shows items in a Vertical List."
        ).c_str()
    );

    m_DetailedListViewToggle = new HighlightToggleButton(this);
    m_DetailedListViewToggle->setIcon(IconForge::GetIcon(IconType::icon_view_stream, _DARK_COLOR(QPalette::Text, 150)));
    m_DetailedListViewToggle->setToolTip(
        ToolTipString(
            "Detailed List View",
            "Shows items in a Vertical List with details."
        ).c_str()
    );

    m_ThumbnailViewToggle = new HighlightToggleButton(this);
    m_ThumbnailViewToggle->setIcon(IconForge::GetIcon(IconType::icon_grid_view, _DARK_COLOR(QPalette::Text, 150)));
    m_ThumbnailViewToggle->setToolTip(
        ToolTipString(
            "Thumbnail View",
            "Shows items in a dynamic Thumbnail Grid."
        ).c_str()
    );

    m_ViewButtonGroup->addButton(m_ListViewToggle, 0);
    m_ViewButtonGroup->addButton(m_DetailedListViewToggle, 1);
    m_ViewButtonGroup->addButton(m_ThumbnailViewToggle, 2);

    m_SearchBar = new MediaSearchBar(this);
    m_SearchBar->setToolTip(
        ToolTipString(
            "Search Media",
            "Searches and filters media in the Media View as per the provided Text."
        ).c_str()
    );

    m_CreateButton = new QPushButton;
    m_CreateButton->setIcon(IconForge::GetIcon(IconType::icon_library_add, palette().color(QPalette::Text).darker(150)));
    m_CreateButton->setFixedWidth(26);
    m_DeleteButton = new QPushButton;
    m_DeleteButton->setIcon(IconForge::GetIcon(IconType::icon_delete, palette().color(QPalette::Text).darker(150)));
    m_DeleteButton->setFixedWidth(26);

    m_OptionsLayout->addWidget(m_ListViewToggle);
    m_OptionsLayout->addWidget(m_DetailedListViewToggle);
    m_OptionsLayout->addWidget(m_ThumbnailViewToggle);
    m_OptionsLayout->addWidget(m_SearchBar);
    m_OptionsLayout->addWidget(m_CreateButton);
    m_OptionsLayout->addWidget(m_DeleteButton);

    /* Setup margins */
    m_OptionsLayout->setContentsMargins(4, 0, 4, 0);
    /* }}} */

    /* Views {{{ */
    m_ViewSplitter = new QSplitter(Qt::Horizontal);

    m_PlaylistView = new PlaylistView(this);
    m_MediaView = new PlaylistMediaView(this);

    m_ViewSplitter->addWidget(m_PlaylistView);
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

void VoidPlayLister::Setup()
{
    SetFromPreferences();

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
}

void VoidPlayLister::Connect()
{
    /* Options */
    connect(m_SearchBar, &MediaSearchBar::typed, m_MediaView, &PlaylistMediaView::Search);
    connect(m_CreateButton, &QPushButton::clicked, this, []() { _MediaBridge.NewPlaylist(); });
    connect(m_DeleteButton, &QPushButton::clicked, m_PlaylistView, &PlaylistView::RemoveSelected);

    /* List */
    connect(m_MediaView, &PlaylistMediaView::itemDoubleClicked, this, &VoidPlayLister::IndexSelected);
    connect(m_PlaylistView, &PlaylistView::itemClicked, this, [this](const QModelIndex& index) { _MediaBridge.SetCurrentPlaylist(index); });
    connect(m_PlaylistView, &PlaylistView::played, this, static_cast<void (VoidPlayLister::*)(Playlist*)>(&VoidPlayLister::Play));
    connect(m_PlaylistView, &PlaylistView::playedAsSequence, this, &VoidPlayLister::PlayAsSequence);
    connect(m_MediaView, &PlaylistMediaView::played, this, static_cast<void (VoidPlayLister::*)(const std::vector<SharedMediaClip>&)>(&VoidPlayLister::Play));
    /* View Changed */
    /* The call to buttonToggled is a slightly expensive as this gets called 2 times if we have n buttons (once for checked off and once for checked on) */
    connect(m_ViewButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton*, bool)>(&QButtonGroup::buttonToggled), this, [this](QAbstractButton* b, bool s)
    {
        if (s)
            m_MediaView->SetViewType(static_cast<PlaylistMediaView::ViewType>(m_ViewButtonGroup->id(b)));
    });
    connect(m_PlaylistView, &PlaylistView::updated, m_MediaView, &PlaylistMediaView::Refresh);


    /* Shortcut */
    connect(m_DeleteShortcut, &QShortcut::activated, this, &VoidPlayLister::RemoveSelectedMedia);
}

void VoidPlayLister::IndexSelected(const QModelIndex& index)
{
    if (!index.isValid())
        return;
 
    _PlayerBridge.SetMedia(*(static_cast<SharedMediaClip*>(index.internalPointer())));
}

void VoidPlayLister::AddSelectionToSequence()
{
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();

    if (selected.empty())
        return;

    std::vector<SharedMediaClip> m;
    /* Already aware of the amount of items which are to be copied */
    m.reserve(selected.size());

    for (const QModelIndex& index: selected)
        m.emplace_back(*(static_cast<SharedMediaClip*>(index.internalPointer())));

    _PlayerBridge.SetMedia(m);
}

void VoidPlayLister::RemoveSelectedMedia()
{
    /* Push all of the selected indexes for removal */
    _MediaBridge.RemoveMedia(m_MediaView->SelectedIndexes());
}

void VoidPlayLister::Play(Playlist* playlist)
{
    /* All of the Media from the playlist as a Queue */
    _PlayerBridge.SetPlaylist(playlist);
}

void VoidPlayLister::PlayAsSequence(Playlist* playlist)
{
    _PlayerBridge.SetMedia(playlist->DataModel()->AllMedia());
}

void VoidPlayLister::Play(const std::vector<SharedMediaClip>& media)
{
    _PlayerBridge.SetMedia(media);
}

void VoidPlayLister::SetFromPreferences()
{
    /* Default View */
    switch(static_cast<PlaylistMediaView::ViewType>(VoidPreferences::Instance().GetMediaViewType()))
    {
        case PlaylistMediaView::ViewType::DetailedListView:
            m_DetailedListViewToggle->setChecked(true);
        case PlaylistMediaView::ViewType::ThumbnailView:
            m_ThumbnailViewToggle->setChecked(true);
        case PlaylistMediaView::ViewType::ListView:
        default:
            m_ListViewToggle->setChecked(true);    
    }
}

VOID_NAMESPACE_CLOSE
