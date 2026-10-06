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
    // if (m_Metadata->IsValid())
    // {
    //     if (m_Type == EntityTagWidget::Type::TRACK)
    //     {
    //         controller->AddTag(static_cast<PlaybackTrack*>(m_Entity), name.toStdString(), m_Metadata->Metadata());
    //     }
    // }
    // else
    // {
    //     if (m_Type == EntityTagWidget::Type::TRACK)
    //     {
    //         controller->AddTag(static_cast<PlaybackTrack*>(m_Entity), name.toStdString());
    //     }
    // }
    if (m_Type == EntityTagWidget::Type::TRACK)
    {
        controller->AddTag(static_cast<PlaybackTrack*>(m_Entity), name.toStdString(), m_Metadata->Metadata());
    }

    accept();
}

/// EntityTagEditor

EntityTagEditor::EntityTagEditor(SequencerContext* context, QWidget* parent)
    : TagEditor(parent)
    , m_Context(context)
{
    m_TagList->setFixedWidth(140);
    connect(m_RemoveButton, &QPushButton::clicked, this, &EntityTagEditor::RemoveSelected);
}

void EntityTagEditor::Set(const SharedPlaybackTrack& track)
{
    m_Entity = track.get();
    m_Type = EntityTagEditor::Type::TRACK;

    if (track->HasTags())
    {
        m_TagList->setModel(track->TagsModel());
        m_TagList->setCurrentIndex(m_TagList->model()->index(0, 0));
        TagSelected(m_TagList->currentIndex());
    }
}

void EntityTagEditor::RemoveSelected()
{
    // const QModelIndex& index = m_TagList->currentIndex();
    // if (index.isValid())
    // {
    //     _MediaBridge.RemoveTag(m_Index, index);
    //     TagSelected(m_TagList->currentIndex());
    // }
}

// void EntityTagEditor::TagSelected(const QModelIndex& index)
// {
//     index.isValid() ? SetCurrentTag(static_cast<Tag*>(index.internalPointer())) : m_TagBase->Reset();
// }

// void EntityTagEditor::SetCurrentTag(const Tag* tag)
// {
//     m_TagBase->ResetName(tag->Name().c_str());
//     m_TagBase->ResetModel(tag->MetadataModel());
// }

VOID_NAMESPACE_CLOSE
