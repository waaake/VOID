// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QGuiApplication>
#include <QLabel>
#include <QScreen>

/* Internal */
#include "TagWidget.h"
#include "VoidIconForge/IconForge.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

/// MediaTagWidget

MediaTagWidget::MediaTagWidget(const QModelIndex& index, QWidget* parent)
    : TagWidget(parent)
    , m_MediaIndex(index)
    , m_Metadata(new TagMetadataModel)
{
    m_TagBase->SetModel(m_Metadata);
    connect(m_AcceptButton, &QPushButton::clicked, this, [this]()
    {
        const QString name = m_TagBase->Name();
        if (name.isEmpty())
            return;

        m_Metadata->IsValid()
            ? _MediaBridge.AddTag(m_MediaIndex, name.toStdString(), m_Metadata->Metadata())
            : _MediaBridge.AddTag(m_MediaIndex, name.toStdString());

        accept();
    });
}

MediaTagWidget::~MediaTagWidget()
{
    if (m_Metadata)
    {
        m_Metadata->deleteLater();
        delete m_Metadata;
        m_Metadata = nullptr;
    }
}

/// MediaTagEditor

MediaTagEditor::MediaTagEditor(const SharedMediaClip& clip, const QModelIndex& index, QWidget* parent)
    : TagEditor(parent)
    , m_Media(clip)
    , m_Index(index)
{
    Setup();
}

void MediaTagEditor::Setup()
{
    connect(m_RemoveButton, &QPushButton::clicked, this, &MediaTagEditor::RemoveSelected);
    if (SharedMediaClip media = m_Media.lock())
    {
        if (media->HasTags())
        {
            SetModel(media->TagsModel());
            ResetTag();
        }
    }
}

void MediaTagEditor::RemoveSelected()
{
    const QModelIndex& index = CurrentTagIndex();
    if (index.isValid())
    {
        _MediaBridge.RemoveTag(m_Index, index);
        SetCurrentTag(CurrentTagIndex());
    }
}

VOID_NAMESPACE_CLOSE
