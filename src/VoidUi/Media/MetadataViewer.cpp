// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QMimeData>

/* Internal */
#include "MetadataViewer.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

MetadataViewer::MetadataViewer(QWidget* parent)
    : QWidget(parent)
{
    Build();

    connect(m_ClearButton, &QPushButton::clicked, m_Tree, &MetadataTree::Clear);
    connect(m_SearchBox, &QLineEdit::textChanged, m_Tree, &MetadataTree::SetSearchKey);
    setAcceptDrops(true);
}

MetadataViewer::~MetadataViewer()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void MetadataViewer::Build()
{
    m_Layout = new QVBoxLayout(this);

    m_ButtonLayout = new QHBoxLayout();

    m_SearchBox = new QLineEdit(this);
    m_SearchBox->setPlaceholderText("Search Metadata");

    m_ClearButton = new QPushButton("Clear", this);

    m_ButtonLayout->addWidget(m_SearchBox);
    m_ButtonLayout->addStretch(1);
    m_ButtonLayout->addWidget(m_ClearButton);

    m_Tree = new MetadataTree(this);

    m_Layout->addLayout(m_ButtonLayout);
    m_Layout->addWidget(m_Tree);
}

void MetadataViewer::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
        event->acceptProposedAction();
}

void MetadataViewer::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        QByteArray data = event->mimeData()->data(MimeTypes::MediaItem);
        const std::vector<SharedMediaClip> media = _MediaBridge.UnpackProjectMedia(data);
        if (!media.empty())
            SetFromMedia(media[0]);
    }
}

VOID_NAMESPACE_CLOSE
