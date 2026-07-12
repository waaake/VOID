// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Sequencer.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SequencerTimeline::SequencerTimeline(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Connect();    
}

void SequencerTimeline::SetSequence(const SharedPlaybackSequence& sequence)
{
    m_Sequence = sequence;

    // m_View->SetSequence(sequence);
    // m_TrackHeader->SetSequence(sequence);
    Refresh();
}

void SequencerTimeline::Refresh()
{
    m_View->SetSequence(m_Sequence);
    m_TrackHeader->SetSequence(m_Sequence);
}

void SequencerTimeline::Build()
{
    m_Layout = new QGridLayout(this);

    m_Toolbar = new SToolbar;
    m_Ruler = new SRulerWidget;
    m_TrackHeader = new STrackHeaderWidget(&m_Context);
    m_View = new STimelineView(&m_Context);

    m_Layout->setSpacing(0);
    m_Layout->setContentsMargins(0, 0, 0, 0);

    m_Layout->addWidget(m_Toolbar, 0, 0);
    m_Layout->addWidget(m_Ruler, 0, 1);

    m_Layout->addWidget(m_TrackHeader, 1, 0);
    m_Layout->addWidget(m_View, 1, 1);

    m_Layout->setColumnMinimumWidth(0, Sequencer::TrackHeaderWidth);
    m_Layout->setRowMinimumHeight(0, Sequencer::RulerHeight);

    m_Layout->setColumnStretch(1, 1);
    m_Layout->setRowStretch(1, 1);
}

void SequencerTimeline::Connect()
{
    // connect(m_Toolbar, &SToolbar::reset, this, [this]() -> void
    // {
    //     VOID_LOG_INFO("Refresh...");
    //     m_View->Clear();
    //     m_TrackHeader->Clear();
    // });
    connect(m_Toolbar, &SToolbar::reset, this, &SequencerTimeline::Refresh);
}

VOID_NAMESPACE_CLOSE
