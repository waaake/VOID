// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLineEdit>
#include <QHeaderView>

/* Internal */
#include "FileView.h"
#include "VoidUi/QExtensions/Delegates.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

static QString s_LastAccessedDir = QDir::currentPath();

FileTree::FileTree(QFileSystemModel* model, QWidget* parent)
    : QTreeView(parent)
    , m_Model(model)
{
    Setup();
    Connect();
}

FileTree::~FileTree()
{
    m_Proxy->deleteLater();
    delete m_Proxy;
    m_Proxy = nullptr;
}

// void FileTree::setRootIndex(const QModelIndex& index)
// {
//     // m_Proxy->ResetSequences();
//     QTreeView::setRootIndex(index);
// }

void FileTree::Setup()
{
    m_Proxy = new MediaFilesProxyModel(this);
    m_Proxy->setSourceModel(m_Model);

    setModel(m_Proxy);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setRootIsDecorated(false);
    setItemsExpandable(false);
    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    header()->setSectionResizeMode(3, QHeaderView::ResizeMode::ResizeToContents);
    setColumnHidden(2, true); /* hide Type column */

    SetDirectory(s_LastAccessedDir);

    setSortingEnabled(true);
    /* Sorting by size gives the best result in segregating folders from files */
    sortByColumn(1, Qt::AscendingOrder);
}

void FileTree::Connect()
{
    connect(
        this, &QTreeView::doubleClicked, this, static_cast<void (FileTree::*)(const QModelIndex&)>(&FileTree::Open)
    );
}

void FileTree::SetRootIndex(const QModelIndex& index)
{
    QFileInfo info = m_Model->fileInfo(m_Proxy->mapToSource(index));

    if (info.isDir())
    {
        setRootIndex(index);
        emit directoryChanged(info.absoluteFilePath());

        s_LastAccessedDir = info.absoluteFilePath();
    }
}

void FileTree::NewDirectory()
{
    QModelIndex index = m_Model->mkdir(m_Proxy->mapToSource(rootIndex()), "New Folder");
    if (index.isValid())
    {
        QModelIndex proxy = m_Proxy->mapFromSource(index);

        setCurrentIndex(proxy);
        selectionModel()->select(proxy, QItemSelectionModel::ClearAndSelect);
        edit(proxy.siblingAtColumn(0));
        // bool status = m_Proxy->setData(proxy, "Tesrrrr", Qt::EditRole);

        // VOID_LOG_INFO("Renamed: {0}", status);
    }
}

void FileTree::SetDirectory(const QString& directory)
{
    QModelIndex index = m_Proxy->mapFromSource(m_Model->index(directory));

    if (index.isValid())
    {
        setRootIndex(index);
        s_LastAccessedDir = directory;
    }
}

QString FileTree::CurrentDirectory() const
{
    return s_LastAccessedDir;
}

void FileTree::CdUp()
{
    QDir d(s_LastAccessedDir);

    if (d.cdUp())
    {
        QModelIndex index = m_Proxy->mapFromSource(m_Model->index(d.absolutePath()));
        if (index.isValid())
        {
            setRootIndex(index);
            s_LastAccessedDir = d.absolutePath();

            emit directoryChanged(d.absolutePath());
        }
    }
}

void FileTree::Open()
{
    QFileInfo entity = m_Model->fileInfo(m_Proxy->mapToSource(currentIndex()));
    entity.isFile() ? emit accepted(entity.absoluteFilePath()) : SetRootIndex(currentIndex());
}

void FileTree::Open(const QModelIndex& index)
{
    QFileInfo entity = m_Model->fileInfo(m_Proxy->mapToSource(index));
    entity.isFile() ? emit accepted(entity.absoluteFilePath()) : SetRootIndex(index);
}

void FileTree::SelectDirectory()
{
    const QModelIndex& index = currentIndex().isValid() ? currentIndex() : rootIndex();
    QFileInfo entity = m_Model->fileInfo(m_Proxy->mapToSource(index));

    if (entity.isDir())
        emit accepted(entity.absoluteFilePath());
}

void FileTree::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    QTreeView::currentChanged(current, previous);
    QFileInfo entity = m_Model->fileInfo(m_Proxy->mapToSource(current));

    if (entity.isFile())
        emit highlighted(current.data(MediaFilesRoles::DisplayNameRole).toString());
}

/* Quick Link View {{{ */

QuickLinkView::QuickLinkView(QWidget* parent)
    : QListWidget(parent)
{
    Setup();
    connect(this, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) -> void
    {
        emit linkSelected(item->data(Qt::UserRole).toString());
    });
}

QuickLinkView::~QuickLinkView()
{
}

void QuickLinkView::Setup()
{
    QListWidgetItem* computer = new QListWidgetItem(style()->standardIcon(style()->SP_ComputerIcon), "Computer");
    computer->setData(Qt::UserRole, QDir::rootPath());
    addItem(computer);

    QListWidgetItem* home = new QListWidgetItem(style()->standardIcon(style()->SP_DirHomeIcon), "Home");
    home->setData(Qt::UserRole, QDir::homePath());
    addItem(home);

    setItemDelegate(new HCustomItemDelegate(26, this));
}

/* }}} */

VOID_NAMESPACE_CLOSE
