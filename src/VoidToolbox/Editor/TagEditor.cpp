// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QGuiApplication>
#include <QLabel>
#include <QScreen>

/* Internal */
#include "TagEditor.h"
#include "VoidIconForge/IconForge.h"
#include "VoidObjects/Media/Tag.h"

VOID_NAMESPACE_OPEN

/// Tag Base

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

void TagBase::Clear()
{
    m_NameEdit->clear();
}

void TagBase::Build()
{
    m_Layout = new QGridLayout(this);

    m_NameEdit = new QLineEdit;
    m_DataTree = new QTreeView;

    m_Layout->addWidget(new QLabel("Name:", this), 0, 0, 1, 1);
    m_Layout->addWidget(m_NameEdit, 0, 1, 1, 2);
    m_Layout->addWidget(new QLabel("Data:", this), 1, 0, 1, 1);
    m_Layout->addWidget(m_DataTree, 1, 1, 7, 2);

    m_Layout->setContentsMargins(0, 0, 0, 0);
}

void TagBase::Setup()
{
    m_DataTree->setAlternatingRowColors(true);
}

/// Tag Widget

TagWidget::TagWidget(QWidget* parent)
    : TranslucentDialog(parent)
{
    Build();
}

TagWidget::~TagWidget()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void TagWidget::MoveTo(const QPoint& position)
{
    QScreen* screen = QGuiApplication::screenAt(position);
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    const QRect bounds = screen->availableGeometry();
    const QSize size = sizeHint();

    // position based on the screen geometry
    move(
        std::max(bounds.left(), std::min(position.x(), bounds.right() - size.width())),
        std::max(bounds.top(), std::min(position.y(), bounds.bottom() - size.height()))
    );
}

void TagWidget::showEvent(QShowEvent* event)
{
    TranslucentDialog::showEvent(event);
    m_TagBase->ResetFocus();
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

/// Tag Editor

TagEditor::TagEditor(QWidget* parent)
    : TranslucentDialog(parent)
{
    Build();

    // Setup
    m_TagList->setFixedWidth(140);
    connect(m_TagList, &QListView::clicked, this, static_cast<void (TagEditor::*)(const QModelIndex&)>(&TagEditor::SetCurrentTag));
}

TagEditor::~TagEditor()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void TagEditor::MoveTo(const QPoint& position)
{
    QScreen* screen = QGuiApplication::screenAt(position);
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    const QRect bounds = screen->availableGeometry();
    const QSize size = sizeHint();

    // position based on the screen geometry
    move(
        std::max(bounds.left(), std::min(position.x(), bounds.right() - size.width())),
        std::max(bounds.top(), std::min(position.y(), bounds.bottom() - size.height()))
    );
}

void TagEditor::SetCurrentTag(const QModelIndex& index)
{
    index.isValid() ? SetCurrentTag(static_cast<Tag*>(index.internalPointer())) : m_TagBase->Clear();
}

void TagEditor::SetCurrentTag(const Tag* tag)
{
    m_TagBase->SetName(tag->Name().c_str());
    m_TagBase->SetModel(tag->MetadataModel());
}

void TagEditor::ResetTag()
{
    m_TagList->setCurrentIndex(m_TagList->model()->index(0, 0));
    SetCurrentTag(m_TagList->currentIndex());
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

VOID_NAMESPACE_CLOSE
