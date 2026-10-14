// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Spreadsheet.h"
#include "FormatForge.h"
#include "VoidConformer/Conformer.h"
#include "VoidSequencer/SContext.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"
#include "VoidMediaPlayer/Player/PlayerBridge.h"
#include "VoidSpreadsheet/Widgets/ConformOptions.h"
#include "VoidQExtensions/MessageBox.h"

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
    connect(m_Sheet, &SpreadsheetTable::itemSelectionChanged, this, [this]() -> void
    {
        m_Context->SelectionModel()->Select(m_Sheet->SelectedItems());
    });
    connect(this, &SpreadsheetWidget::conformMedia, this, &Spreadsheet::Conform);
    connect(this, &SpreadsheetWidget::updateReferenceMedia, this, &Spreadsheet::SetReferenceMedia);
}

void Spreadsheet::Conform(const std::vector<SharedMediaClip>& clips)
{
    if (const SharedPlaybackSequence& sequence = m_Context->Sequence())
    {
        ConformOptionsDialog opts(this);
        if (!opts.exec()) return;

        const std::vector<SharedTrackItem> items = sequence->VideoTrackItems();

        Conformer conformer(clips, items, _MediaBridge.ActiveProject());
        std::unordered_set<SharedTrackItem> linked = conformer.Conform(opts.Options());

        QString text("Reconnected %1/%2 clips");
        InfoMessageBox ibox(text.arg(static_cast<int>(linked.size())).arg(static_cast<int>(items.size())));
        ibox.exec();
    }
}

void Spreadsheet::SetReferenceMedia(const SharedMediaClip& media)
{
    if (SharedPlaybackSequence sequence = m_Context->Sequence())
        m_Context->Controller()->SetReferenceMedia(media, sequence);
}

VOID_NAMESPACE_CLOSE
