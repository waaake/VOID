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
    m_MediaView->deleteLater();
    delete m_MediaView;
    m_MediaView = nullptr;
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
            emit mediaDropped(path);
        }
    }
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

    /* Add to the base Layout */
    m_layout->addLayout(m_OptionsLayout);
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

    /* List */
    connect(m_MediaView, &MediaView::itemDoubleClicked, this, &VoidMediaLister::IndexSelected);
    connect(m_MediaView, &MediaView::customContextMenuRequested, this, &VoidMediaLister::ShowContextMenu);
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

    for (const QModelIndex& index: selected)
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

    /* Show Menu */
    contextMenu.exec(m_MediaView->mapToGlobal(position));
}

void VoidMediaLister::RemoveSelectedMedia()
{
    /* Selected Indexes */
    std::vector<QModelIndex> selected = m_MediaView->SelectedIndexes();

    /**
     * Loop over in a a reverse way as forward iteration would shift the model indexes and
     * result in wrong indexes being deleted, or a worst case scenario result in crashes as
     * the second model index doesn't even exist after the first has been deleted
     */
    for (int i = selected.size() - 1; i >= 0; --i)
    {
        MBridge::Instance().Remove(selected.at(i));
    }
}

VOID_NAMESPACE_CLOSE
