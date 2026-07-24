// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QColorDialog>
#include <QScrollBar>

/* Internal */
#include "Sequencer.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SequencerTimeline::SequencerTimeline(QWidget* parent)
    : QWidget(parent)
    , m_Sequence(nullptr)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    Build();
    Connect();
}

void SequencerTimeline::SetFrame(v_frame_t frame)
{
    m_Context.Controller()->SetCurrentFrame(frame);
    VOID_LOG_INFO("SequencerTimeline::SetFrame::{0}", frame);
}

void SequencerTimeline::SetSequence(const SharedPlaybackSequence& sequence)
{
    if (m_Sequence)
    {
        disconnect(m_Sequence.get(), &PlaybackSequence::trackAdded, this, &SequencerTimeline::AddTrack);
        disconnect(m_Sequence.get(), &PlaybackSequence::trackAboutToBeRemoved, this, &SequencerTimeline::RemoveTrack);
    }

    m_Sequence = sequence;
    connect(m_Sequence.get(), &PlaybackSequence::trackAdded, this, &SequencerTimeline::AddTrack);
    connect(m_Sequence.get(), &PlaybackSequence::trackAboutToBeRemoved, this, &SequencerTimeline::RemoveTrack);

    // m_View->SetSequence(sequence);
    // m_TrackHeader->SetSequence(sequence);
    Refresh();
}

void SequencerTimeline::Refresh()
{
    m_TrackHeader->Clear();
    m_View->Clear();

    m_View->AddPlayhead();

    for (int i = 0; i < m_Sequence->NumVideoTracks(); ++i)
        AddTrack(m_Sequence->VideoTrackAt(i));
}

void SequencerTimeline::AddTrack(const SharedPlaybackTrack& track)
{
    m_View->AddTrack(track);
    m_TrackHeader->AddTrack(track);
}

void SequencerTimeline::RemoveTrack(const SharedPlaybackTrack& track)
{
    m_TrackHeader->RemoveTrack(track);
    m_View->RemoveTrack(track);
}

void SequencerTimeline::Build()
{
    m_FitShortcut = new QShortcut(QKeySequence("Alt+F"), this);
    m_DeleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    m_Menu = new SequencerContextMenu(&m_Context, this);

    m_Layout = new QHBoxLayout(this);

    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(0);
    grid->setContentsMargins(0, 0, 0, 0);

    m_Toolbar = new SToolbar;

    m_TrackHeader = new STrackHeaderWidget(&m_Context);
    m_View = new STimelineView(&m_Context);

    m_Ruler = new STimelineRuler(m_View, &m_Context);

    grid->addWidget(m_Ruler, 0, 1);

    grid->addWidget(m_TrackHeader, 1, 0);
    grid->addWidget(m_View, 1, 1);

    grid->setColumnMinimumWidth(0, Sequencer::TrackHeaderWidth);
    grid->setRowMinimumHeight(0, Sequencer::RulerHeight);

    grid->setColumnStretch(1, 1);
    grid->setRowStretch(1, 1);

    m_Layout->setSpacing(0);
    m_Layout->setContentsMargins(0, 0, 0, 0);

    m_Layout->addWidget(m_Toolbar);
    m_Layout->addLayout(grid);
}

void SequencerTimeline::Connect()
{
    connect(m_Toolbar, &SToolbar::reset, this, &SequencerTimeline::Refresh);

    connect(m_FitShortcut, &QShortcut::activated, m_View, &STimelineView::Focus);
    connect(m_DeleteShortcut, &QShortcut::activated, this, &SequencerTimeline::DeleteSelected);

    connect(m_Context.Controller(), &SequencerController::frameChangeRequested, this, &SequencerTimeline::frameChangeRequested);
    connect(this, &QWidget::customContextMenuRequested, this, [this](const QPoint& position) -> void 
    {
        m_Menu->Show(mapToGlobal(position));
    });
    connect(m_View->verticalScrollBar(), &QScrollBar::valueChanged, m_TrackHeader, &STrackHeaderWidget::SetScroll);

    connect(m_Menu, &SequencerContextMenu::createTrackRequested, this, [this]() -> void
    {
        m_Context.Controller()->CreateVideoTrack(m_Sequence);
    });
    connect(m_Menu, &SequencerContextMenu::removeTracksRequested, this, &SequencerTimeline::DeleteSelected);
    connect(m_Menu, &SequencerContextMenu::colorChangeRequested, this, [this](bool reset) -> void
    {
        if (reset)
        {
            m_Context.Controller()->SetTrackItemsColor(m_Context.SelectionModel()->SelectedItems());
        }
        else
        {
            QColor color = QColorDialog::getColor(QColor(255, 255, 255), this, "Select Trackitem Color");
            m_Context.Controller()->SetTrackItemsColor(m_Context.SelectionModel()->SelectedItems(), color);    
        }
    });
}

void SequencerTimeline::DeleteSelected()
{
    if (m_Context.SelectionModel()->HasTrackSelection())
        m_Context.Controller()->RemoveTracks(m_Sequence, m_Context.SelectionModel()->SelectedTracks());
    else if (m_Context.SelectionModel()->HasTrackSelection())
        m_Context.Controller()->RemoveTrackItems(m_Sequence, m_Context.SelectionModel()->SelectedItems());
    
    m_Context.SelectionModel()->Clear();
}

VOID_NAMESPACE_CLOSE
