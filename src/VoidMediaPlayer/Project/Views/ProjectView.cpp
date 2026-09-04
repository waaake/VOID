// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QMenu>

/* Internal */
#include "ProjectView.h"
#include "VoidCore/Logging.h"
#include "VoidMediaPlayer/Project/Delegates/ListDelegate.h"
#include "VoidMediaPlayer/Project/ProjectBridge.h"

VOID_NAMESPACE_OPEN

ProjectView::ProjectView(QWidget* parent)
    : QListView(parent)
{
    Setup();
    Connect();
}

ProjectView::~ProjectView()
{
    /**
     * Set the source Model as nullpointer so that we don't actually delete
     * the original source model
     */
    proxy->setSourceModel(nullptr);

    proxy->deleteLater();
    delete proxy;
    proxy = nullptr;

    m_NewMenu->deleteLater();
    delete m_NewMenu;
    m_NewMenu = nullptr;

    m_ImportMenu->deleteLater();
    delete m_ImportMenu;
    m_ImportMenu = nullptr;
}

void ProjectView::Setup()
{
    m_NewMenu = new QMenu("New");
    m_CreateSequenceAction = new QAction("New Sequence", m_NewMenu);
    m_NewMenu->addAction(m_CreateSequenceAction);

    m_ImportMenu = new QMenu("Import");
    m_ImportMediaAction = new QAction("Import Media...", m_ImportMenu);
    m_ImportDirectoryAction = new QAction("Import Directory...", m_ImportMenu);
    m_ImportMenu->addAction(m_ImportMediaAction);
    m_ImportMenu->addAction(m_ImportDirectoryAction);

    m_SaveProjectAction = new QAction("Save Project", this);
    m_SaveAsProjectAction = new QAction("Save Project As...", this);
    m_CloseProjectAction = new QAction("Close Project", this);

    ProjectModel* model = _MediaBridge.ProjectDataModel();
    proxy = new ProjectProxyModel(this);
    ResetModel(model);
    setModel(proxy);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformItemSizes(true);
    setItemDelegate(new ProjectItemDelegate(this));
    setSpacing(1);

    setContextMenuPolicy(Qt::CustomContextMenu);
}

void ProjectView::Connect()
{
    connect(m_ImportMediaAction, &QAction::triggered, this, &ProjectView::ImportMedia);
    connect(m_ImportDirectoryAction, &QAction::triggered, this, &ProjectView::ImportDirectory);
    connect(m_CreateSequenceAction, &QAction::triggered, this, &ProjectView::AddSequence);
    connect(m_SaveProjectAction, &QAction::triggered, this, [this]() -> void { SaveProject(); });
    connect(m_SaveAsProjectAction, &QAction::triggered, this, [this]() -> void { SaveProject(true); });
    connect(m_CloseProjectAction, &QAction::triggered, this, &ProjectView::CloseProject);

    connect(this, &QListView::clicked, this, &ProjectView::ItemClicked);
    connect(this, &QListView::customContextMenuRequested, this, &ProjectView::ShowContextMenu);
}

void ProjectView::ResetModel(ProjectModel* model)
{
    proxy->setSourceModel(model);
    VOID_LOG_INFO("Project Source Model Updated");
}

void ProjectView::ItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    /* The source index */
    emit itemClicked(proxy->mapToSource(index));
}

const std::vector<QModelIndex> ProjectView::SelectedIndexes() const
{
    std::vector<QModelIndex> sources;
    QItemSelectionModel* selection = selectionModel();

    // Nothing is selected at the moment
    if (!selection)
        return sources;

    const QModelIndexList proxyindexes = selection->selectedRows();
    sources.reserve(proxyindexes.size());
    for (const QModelIndex& index: proxyindexes)
    {
        QModelIndex source = proxy->mapToSource(index);
        if (source.isValid())
            sources.emplace_back(source);
    }

    return sources;
}

bool ProjectView::HasSelection()
{
    QItemSelectionModel* s = selectionModel();
    return s && s->hasSelection();
}

void ProjectView::EnableSorting(bool state, const Qt::SortOrder& order)
{
    proxy->sort(state ? 0 : -1, order);
}

void ProjectView::ShowContextMenu(const _QPoint& position)
{
    if (!HasSelection())
        return;

    QMenu contextMenu(this);
    
    contextMenu.addMenu(m_NewMenu);
    contextMenu.addMenu(m_ImportMenu);
    contextMenu.addSeparator();
    contextMenu.addAction(m_SaveProjectAction);
    contextMenu.addAction(m_SaveAsProjectAction);
    contextMenu.addAction(m_CloseProjectAction);

    /* Show Menu */
    #if _QT6
    /**
     * Qt6 mapToGlobal returns QPointF while menu.exec expects QPoint
     */
    contextMenu.exec(mapToGlobal(position).toPoint());
    #else
    contextMenu.exec(mapToGlobal(position));
    #endif // _QT6
}

Project* ProjectView::HighlightedProject()
{
    QItemSelectionModel* selection = selectionModel();
    if (!selection)
        return nullptr;

    return _ProjectBridge.ProjectAt(selection->currentIndex());
}

void ProjectView::ImportMedia()
{
    if (Project* project = HighlightedProject())
        _ProjectBridge.ImportMedia(project);
}

void ProjectView::ImportDirectory()
{
    if (Project* project = HighlightedProject())
        _ProjectBridge.ImportDirectory(project);   
}

void ProjectView::AddSequence()
{
    if (Project* project = HighlightedProject())
        _ProjectBridge.AddSequence(project);
}

void ProjectView::SaveProject(bool saveas)
{
    if (Project* project = HighlightedProject())
        saveas ? _ProjectBridge.SaveAs(project) : _ProjectBridge.Save(project);
}

void ProjectView::CloseProject()
{
    if (Project* project = HighlightedProject())
        _ProjectBridge.Close(project);
}

VOID_NAMESPACE_CLOSE
