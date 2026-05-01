// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QGuiApplication>
#include <QLabel>
#include <QScreen>

/* Internal */
#include "TagWidget.h"
#include "VoidUi/Engine/IconForge.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

/* Tag Base {{{ */

TagBase::TagBase(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Setup();
}

TagBase::~TagBase()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void TagBase::Reset()
{
    m_NameEdit->clear();
}

void TagBase::Build()
{
    m_Layout = new QGridLayout(this);

    QLabel* nameLabel = new QLabel("Name:");
    QLabel* dataLabel = new QLabel("Data:");

    m_NameEdit = new QLineEdit;
    m_DataTree = new QTreeView;

    m_Layout->addWidget(nameLabel, 0, 0, 1, 1);
    m_Layout->addWidget(m_NameEdit, 0, 1, 1, 2);
    m_Layout->addWidget(dataLabel, 1, 0, 1, 1);
    m_Layout->addWidget(m_DataTree, 1, 1, 7, 2);

    m_Layout->setContentsMargins(0, 0, 0, 0);
}

void TagBase::Setup()
{
    m_DataTree->setAlternatingRowColors(true);
}

/* }}} */

/* Tag Widget {{{ */

TagWidget::TagWidget(const QModelIndex& index, QWidget* parent)
    : TranslucentDialog(parent)
    , m_MediaIndex(index)
    , m_Metadata(new TagMetadataModel)
{
    Build();
    Setup();
    Connect();
}

TagWidget::~TagWidget()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;

    if (m_Metadata)
    {
        m_Metadata->deleteLater();
        delete m_Metadata;
        m_Metadata = nullptr;
    }
}

void TagWidget::MoveTo(const QPoint& position)
{
    if (position.y() > (QGuiApplication::primaryScreen()->geometry().height() * 0.5))
        move({position.x(), position.y() - sizeHint().height()});
    else
        move(position);
}

void TagWidget::showEvent(QShowEvent* event)
{
    TranslucentDialog::showEvent(event);
    m_TagBase->m_NameEdit->setFocus();
}

void TagWidget::Build()
{
    m_Layout = new QVBoxLayout(this);

    QGridLayout* internalLayout = new QGridLayout;
    m_TagBase = new TagBase(this);

    m_AcceptButton = new QPushButton("Ok");

    internalLayout->addWidget(m_TagBase, 0, 0, 4, 3);
    internalLayout->addWidget(m_AcceptButton, 4, 2, 1, 1);

    m_Layout->addLayout(internalLayout);
}

void TagWidget::Setup()
{
    m_TagBase->m_DataTree->setModel(m_Metadata);
}

void TagWidget::Connect()
{
    connect(m_AcceptButton, &QPushButton::clicked, this, [this]()
    {
        const QString name = m_TagBase->m_NameEdit->text();
        if (name.isEmpty())
            return;

        m_Metadata->IsValid()
            ? _MediaBridge.AddTag(m_MediaIndex, name.toStdString(), m_Metadata->Metadata())
            : _MediaBridge.AddTag(m_MediaIndex, name.toStdString());

        accept();
    });
}

/* }}} */

/* Tag Editor {{{ */

TagEditor::TagEditor(const SharedMediaClip& clip, const QModelIndex& index, QWidget* parent)
    : TranslucentDialog(parent)
    , m_Media(clip)
    , m_Index(index)
{
    Build();
    Setup();
    Connect();
}

TagEditor::~TagEditor()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void TagEditor::MoveTo(const QPoint& position)
{
    if (position.y() > (QGuiApplication::primaryScreen()->geometry().height() * 0.5))
        move({position.x(), position.y() - sizeHint().height()});
    else
        move(position);
}

void TagEditor::Build()
{
    m_Layout = new QVBoxLayout(this);

    QHBoxLayout* internalLayout = new QHBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;

    m_TagList = new QListView(this);
    m_TagBase = new TagBase(this);
    m_RemoveButton = new QPushButton;
    m_RemoveButton->setIcon(IconForge::GetIcon(IconType::icon_remove, _DARK_COLOR(QPalette::Text, 100)));
    m_RemoveButton->setFixedWidth(36);

    buttonLayout->addWidget(m_RemoveButton);
    buttonLayout->addStretch(1);

    internalLayout->addWidget(m_TagList);
    internalLayout->addWidget(m_TagBase);

    m_Layout->addLayout(internalLayout);
    m_Layout->addLayout(buttonLayout);
}

void TagEditor::Setup()
{
    m_TagList->setFixedWidth(140);
    if (SharedMediaClip media = m_Media.lock())
    {
        if (media->HasTags())
        {
            m_TagList->setModel(media->TagsModel());
            m_TagList->setCurrentIndex(m_TagList->model()->index(0, 0));
            TagSelected(m_TagList->currentIndex());
        }
    }
}

void TagEditor::Connect()
{
    connect(m_TagList, &QListView::clicked, this, &TagEditor::TagSelected);
    connect(m_RemoveButton, &QPushButton::clicked, this, &TagEditor::RemoveSelected);
}

void TagEditor::RemoveSelected()
{
    const QModelIndex& index = m_TagList->currentIndex();
    if (index.isValid())
    {
        _MediaBridge.RemoveTag(m_Index, index);
        TagSelected(m_TagList->currentIndex());
    }
}

void TagEditor::TagSelected(const QModelIndex& index)
{
    index.isValid() ? SetCurrentTag(static_cast<Tag*>(index.internalPointer())) : m_TagBase->Reset();
}

void TagEditor::SetCurrentTag(const Tag* tag)
{
    m_TagBase->m_NameEdit->setText(tag->Name().c_str());
    m_TagBase->m_DataTree->setModel(tag->MetadataModel());
}

/* }}} */

VOID_NAMESPACE_CLOSE
