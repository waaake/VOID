// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QColorDialog>
#include <QScrollBar>
#include <QStyle>

/* Internal */
#include "Sequencer.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SequencerTimeline::SequencerTimeline(TimelineController* controller, QWidget* parent)
    : QWidget(parent)
    , m_Sequence(nullptr)
{
    m_Context.Controller()->SetTimeController(controller);
    setContextMenuPolicy(Qt::CustomContextMenu);

    Build();
    Connect();
}

void SequencerTimeline::SetSequence(const SharedPlaybackSequence& sequence)
{
    if (m_Sequence)
    {
        disconnect(m_Sequence.get(), &PlaybackSequence::trackAdded, this, &SequencerTimeline::AddTrack);
        disconnect(m_Sequence.get(), &PlaybackSequence::trackAboutToBeRemoved, this, &SequencerTimeline::RemoveTrack);
        disconnect(m_Sequence.get(), &PlaybackSequence::maxTrackEffectsChanged, this, &SequencerTimeline::UpdateAll);
    }

    m_Sequence = sequence;
    connect(m_Sequence.get(), &PlaybackSequence::trackAdded, this, &SequencerTimeline::AddTrack);
    connect(m_Sequence.get(), &PlaybackSequence::trackAboutToBeRemoved, this, &SequencerTimeline::RemoveTrack);
    connect(m_Sequence.get(), &PlaybackSequence::maxTrackEffectsChanged, this, &SequencerTimeline::UpdateAll);

    m_Context.Geometry()->SetSequence(sequence);
    Refresh();
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

void SequencerTimeline::RazorAt(v_frame_t frame)
{
    if (m_Sequence) m_Context.Controller()->RazorAt(m_Sequence, frame);
}

void SequencerTimeline::RazorAt(const SharedPlaybackSequence& sequence, v_frame_t frame)
{
    m_Context.Controller()->RazorAt(sequence, frame);
}

void SequencerTimeline::RazorAt(const SharedPlaybackTrack& track, v_frame_t frame)
{
    m_Context.Controller()->RazorAt(track, frame);
}

void SequencerTimeline::MergeCut(const SharedPlaybackTrack& track, v_frame_t frame)
{
    m_Context.Controller()->MergeCut(track, frame);
}

void SequencerTimeline::TrimItemHead(const SharedTrackItem& item, int handle)
{
    m_Context.Controller()->TrimItemHead(item, handle);
}

void SequencerTimeline::TrimItemTail(const SharedTrackItem& item, int handle)
{
    m_Context.Controller()->TrimItemTail(item, handle);
}

void SequencerTimeline::Refresh()
{
    m_TrackHeader->Clear();
    m_View->Clear();

    m_View->AddPlayhead();

    for (int i = 0; i < m_Sequence->NumVideoTracks(); ++i)
        AddTrack(m_Sequence->VideoTrackAt(i));
}

void SequencerTimeline::SetHorizontalScale(float factor)
{
    m_Context.Geometry()->SetPixelsPerFrame(factor);
    m_View->Refresh();
    m_Ruler->Update();
}

void SequencerTimeline::Build()
{
    m_FitShortcut = new QShortcut(QKeySequence("Alt+F"), this);
    m_DeleteShortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
    m_RippleDeleteShortcut = new QShortcut(QKeySequence("Ctrl+Backspace"), this);
    m_ToggleStateShortcut = new QShortcut(QKeySequence(Qt::Key_D), this);
    m_Menu = new SequencerContextMenu(&m_Context, this);

    m_Layout = new QHBoxLayout(this);

    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(0);
    grid->setContentsMargins(0, 0, 0, 0);

    m_Toolbar = new SToolbar;

    m_TrackHeader = new STrackHeaderWidget(&m_Context);

    m_HZoomSlider = new QSlider(Qt::Horizontal, this);
    m_HZoomSlider->setFixedHeight(style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2);
    m_HZoomSlider->setMinimum(1);
    m_HZoomSlider->setMaximum(200);
    m_HZoomSlider->setValue(m_Context.Geometry()->PixelsPerFrame() * 10);

    m_View = new STimelineView(&m_Context);
    m_Ruler = new STimelineRuler(m_View, &m_Context);

    grid->addWidget(m_Ruler, 0, 1);

    grid->addWidget(m_TrackHeader, 1, 0);
    grid->addWidget(m_HZoomSlider, 2, 0);
    grid->addWidget(m_View, 1, 1, 2, 1);

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
    connect(m_Toolbar, &SToolbar::actionSwitched, this, [this](const SequencerAction& action) -> void { m_Context.SetAction(action); });

    // Controller
    connect(m_Context.Controller(), &SequencerController::editEffectRequested, this, &SequencerTimeline::editEffectRequested);

    connect(m_FitShortcut, &QShortcut::activated, m_View, &STimelineView::Focus);
    connect(m_DeleteShortcut, &QShortcut::activated, this, &SequencerTimeline::DeleteSelected);
    connect(m_RippleDeleteShortcut, &QShortcut::activated, this, &SequencerTimeline::RippleDeleteSelected);
    connect(m_ToggleStateShortcut, &QShortcut::activated, this, &SequencerTimeline::ToggleItemState);

    connect(m_HZoomSlider, &QSlider::valueChanged, this, [this](int value) -> void
    {
        SetHorizontalScale((float)value / 10);
    });

    connect(this, &QWidget::customContextMenuRequested, this, [this](const QPoint& position) -> void
    {
        m_Menu->Show(mapToGlobal(position));
    });
    connect(m_View->verticalScrollBar(), &QScrollBar::valueChanged, m_TrackHeader, &STrackHeaderWidget::SetScroll);
    connect(m_View, &STimelineView::sequenceCutRequested, this, static_cast<void (SequencerTimeline::*)(v_frame_t)>(&SequencerTimeline::RazorAt));

    connect(m_Menu, &SequencerContextMenu::createTrackRequested, this, [this]() -> void
    {
        m_Context.Controller()->CreateVideoTrack(m_Sequence);
    });
    connect(m_Menu, &SequencerContextMenu::removeTracksRequested, this, &SequencerTimeline::DeleteSelected);
    connect(m_Menu, &SequencerContextMenu::removeTrackItemsRequested, this, &SequencerTimeline::DeleteSelected);
    connect(m_Menu, &SequencerContextMenu::editModeChangeRequested, m_Context.Controller(), &SequencerController::SetEditMode);
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
    else if (m_Context.SelectionModel()->HasTrackItemSelection())
        m_Context.Controller()->RemoveTrackItems(m_Sequence, m_Context.SelectionModel()->SelectedItems());
    else if (m_Context.SelectionModel()->HasEffectSelection())
        m_Context.Controller()->RemoveTimelineEffects(m_Context.SelectionModel()->SelectedEffects());

    m_Context.SelectionModel()->Clear();
}

void SequencerTimeline::RippleDeleteSelected()
{
    if (m_Context.SelectionModel()->HasTrackItemSelection())
        m_Context.Controller()->RippleRemoveTrackItems(m_Sequence, m_Context.SelectionModel()->SelectedItems());

    m_Context.SelectionModel()->Clear();
}

void SequencerTimeline::ToggleItemState()
{
    if (m_Context.SelectionModel()->HasTrackItemSelection())
        m_Context.Controller()->ToggleItemState(m_Context.SelectionModel()->SelectedItems());

    if (m_Context.SelectionModel()->HasEffectSelection())
        m_Context.Controller()->ToggleItemState(m_Context.SelectionModel()->SelectedEffects());
}

void SequencerTimeline::UpdateAll()
{
    m_TrackHeader->Update();
    m_View->Refresh();
}

VOID_NAMESPACE_CLOSE
