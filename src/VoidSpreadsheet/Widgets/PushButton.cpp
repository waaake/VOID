// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

/* Internal */
#include "PushButton.h"
#include "Internal/Descriptors.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

MediaDropButton::MediaDropButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
{
    setAcceptDrops(true);
}

void MediaDropButton::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem) || event->mimeData()->hasFormat(MimeTypes::PlaylistItem))
        event->acceptProposedAction();
}

void MediaDropButton::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        QByteArray data = event->mimeData()->data(MimeTypes::MediaItem);
        const std::vector<SharedMediaClip> media = _MediaBridge.UnpackProjectMedia(data);
        emit mediaDropped(media);
    }
}

VOID_NAMESPACE_CLOSE
