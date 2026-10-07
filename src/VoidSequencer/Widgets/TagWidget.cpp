// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QApplication>
#include <QLabel>
#include <QScreen>

/* Internal */
#include "TagWidget.h"

VOID_NAMESPACE_OPEN

/// EntityTagWidget

EntityTagWidget::EntityTagWidget(SequencerContext* context, QWidget* parent)
    : TagWidget(parent)
    , m_Metadata(new TagMetadataModel)
    , m_Context(context)
{
    m_TagBase->SetModel(m_Metadata);
    connect(m_AcceptButton, &QPushButton::clicked, this, &EntityTagWidget::AddTag);
}

EntityTagWidget::~EntityTagWidget()
{
    if (m_Metadata)
    {
        m_Metadata->deleteLater();
        delete m_Metadata;
        m_Metadata = nullptr;
    }
}

void EntityTagWidget::Set(const SharedTrackItem& item)
{
    m_Entity = item.get();
    m_Type = EntityTagWidget::Type::TRACK_ITEM;
}

void EntityTagWidget::Set(const SharedPlaybackTrack& track)
{
    m_Entity = track.get();
    m_Type = EntityTagWidget::Type::TRACK;
}

void EntityTagWidget::AddTag()
{
    const QString& name = m_TagBase->Name();
    if (name.isEmpty())
        return;

    SequencerController* controller = m_Context->Controller();
    switch (m_Type)
    {
        case EntityTagWidget::Type::TRACK:
            controller->AddTag(static_cast<PlaybackTrack*>(m_Entity), name.toStdString(), m_Metadata->Metadata());
            break;
        case EntityTagWidget::Type::TRACK_ITEM:
            controller->AddTag(static_cast<TrackItem*>(m_Entity), name.toStdString(), m_Metadata->Metadata());
            break;
    }

    accept();
}

/// EntityTagEditor

EntityTagEditor::EntityTagEditor(SequencerContext* context, QWidget* parent)
    : TagEditor(parent)
    , m_Context(context)
{
    connect(m_RemoveButton, &QPushButton::clicked, this, &EntityTagEditor::RemoveSelected);
}

void EntityTagEditor::Set(const SharedTrackItem& item)
{
    m_Entity = item.get();
    m_Type = EntityTagEditor::Type::TRACK_ITEM;

    if (item->HasTags())
    {
        SetModel(item->TagsModel());
        ResetTag();
    }
}

void EntityTagEditor::Set(const SharedPlaybackTrack& track)
{
    m_Entity = track.get();
    m_Type = EntityTagEditor::Type::TRACK;

    if (track->HasTags())
    {
        SetModel(track->TagsModel());
        ResetTag();
    }
}

void EntityTagEditor::RemoveSelected()
{
    const QModelIndex& index = CurrentTagIndex();
    if (index.isValid())
    {
        SequencerController* controller = m_Context->Controller();
        switch (m_Type)
        {
            case EntityTagEditor::Type::TRACK:
                controller->RemoveTag(static_cast<PlaybackTrack*>(m_Entity), index);
                break;
            case EntityTagEditor::Type::TRACK_ITEM:
                controller->RemoveTag(static_cast<TrackItem*>(m_Entity), index);
                break;
        }
        SetCurrentTag(CurrentTagIndex());
    }
}

VOID_NAMESPACE_CLOSE
