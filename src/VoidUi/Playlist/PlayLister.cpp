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
#include "VoidUi/QExtensions/Tooltip.h"
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
            // MBridge::Instance().ImportDirectory(path, false);
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

    m_SearchBar = new MediaSearchBar(this);
    m_SearchBar->setToolTip(
        ToolTipString(
            "Search Media",
            "Searches and filters media in the Media View as per the provided Text."
        ).c_str()
    );

    m_CreateButton = new QPushButton;
    m_CreateButton->setIcon(QIcon(":resources/icons/icon_add.svg"));
    m_CreateButton->setFixedWidth(26);
    m_DeleteButton = new QPushButton;
    m_DeleteButton->setIcon(QIcon(":resources/icons/icon_delete.svg"));
    m_DeleteButton->setFixedWidth(26);

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
    connect(m_CreateButton, &QPushButton::clicked, this, []() { MBridge::Instance().NewPlaylist(); });
    connect(m_DeleteButton, &QPushButton::clicked, m_PlaylistView, &PlaylistView::RemoveSelected);

    /* List */
    connect(m_MediaView, &PlaylistMediaView::itemDoubleClicked, this, &VoidPlayLister::IndexSelected);
    connect(m_PlaylistView, &PlaylistView::itemClicked, this, [this](const QModelIndex& index) { MBridge::Instance().SetCurrentPlaylist(index); });
    connect(m_PlaylistView, &PlaylistView::played, this, static_cast<void (VoidPlayLister::*)(const Playlist*)>(&VoidPlayLister::Play));
    connect(m_MediaView, &PlaylistMediaView::played, this, static_cast<void (VoidPlayLister::*)(const std::vector<SharedMediaClip>&)>(&VoidPlayLister::Play));

    /* Shortcut */
    connect(m_DeleteShortcut, &QShortcut::activated, this, &VoidPlayLister::RemoveSelectedMedia);
}

void VoidPlayLister::IndexSelected(const QModelIndex& index)
{
    if (!index.isValid())
        return;
 
    /* Emit the Media Clip dereferenced from the internal pointer */
    emit mediaChanged(*(static_cast<SharedMediaClip*>(index.internalPointer())));
}

void VoidPlayLister::AddSelectionToSequence()
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

    /* Emit that the sequence of playing media is now changed */
    emit playlistChanged(m);
}

void VoidPlayLister::RemoveSelectedMedia()
{
    /* Push all of the selected indexes for removal */
    MBridge::Instance().RemoveMedia(m_MediaView->SelectedIndexes());
}

void VoidPlayLister::Play(const Playlist* playlist)
{
    /* All of the Media from the playlist */
    emit playlistChanged(playlist->DataModel()->AllMedia());
}

void VoidPlayLister::Play(const std::vector<SharedMediaClip>& media)
{
    emit playlistChanged(media);
}

VOID_NAMESPACE_CLOSE
