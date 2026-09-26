// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Spreadsheet.h"
#include "VoidSequencer/SContext.h"
#include "VoidMediaPlayer/Player/PlayerBridge.h"

VOID_NAMESPACE_OPEN

Spreadsheet::Spreadsheet(QWidget* parent)
    : SpreadsheetWidget(parent)
{
    Setup();
}

Spreadsheet::~Spreadsheet()
{
}

void Spreadsheet::SetContext(SequencerContext* context)
{
    m_Context = context;
}

void Spreadsheet::SetSequence(const SharedPlaybackSequence& sequence)
{
    m_Sheet->ResetSequence(sequence);
}

void Spreadsheet::Setup()
{
    /// PlayerBridge
    connect(&_PlayerBridge, &PlayerBridge::playComponentUpdated, this, [this](const PlayerBuffer::PlayableComponent& component) -> void
    {
        if (component == PlayerBuffer::PlayableComponent::Sequence)
        {
            ViewerBuffer* viewer = _PlayerBridge.ActiveViewer();
            SetSequence(viewer->GetSequence());
        }
    });

    /// View
    connect(m_Sheet, &SpreadsheetTable::itemSelected, this, [this](const SharedTrackItem& item) -> void
    {
        m_Context->SelectionModel()->Select(m_Sheet->SelectedItems());
    });
}

VOID_NAMESPACE_CLOSE
