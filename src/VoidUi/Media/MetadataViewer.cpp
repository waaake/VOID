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
    m_ClearButton = new QPushButton("Clear");

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

        /* Read Input data */
        QDataStream stream(&data, QIODevice::ReadOnly);
        int row, column;
        stream >> row >> column;

        /**
         * Media from the Media Bridge
         * The media is always retrieved from the active project
         * the assumption is that a drag-drop event would always happen when the project is active
         */
        SharedMediaClip media = _MediaBridge.MediaAt(row, column);
        SetFromMedia(media);
    }
}

VOID_NAMESPACE_CLOSE
