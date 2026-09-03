// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QColorDialog>
#include <QCursor>
#include <QScrollBar>
#include <QStyle>

/* Internal */
#include "Sequencer.h"
#include "VoidCore/Logging.h"
#include "VoidMediaPlayer/Player/PlayerBridge.h"
#include "VoidObjects/Sequence/Context.h"
#include "VoidSequencer/Graphics/STrack.h"

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
        Disconnect(m_Sequence.get());

    m_Sequence = sequence;
    Connect(m_Sequence.get());

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
    m_CutShortcut = new QShortcut(QKeySequence::Cut, this);
    m_CutShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    m_CopyShortcut = new QShortcut(QKeySequence::Copy, this);
    m_CopyShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    m_PasteShortcut = new QShortcut(QKeySequence::Paste, this);
    m_PasteShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    m_FitShortcut = new QShortcut(QKeySequence("Alt+F"), this);
    m_FitShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    m_DeleteShortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
    m_DeleteShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    m_RippleDeleteShortcut = new QShortcut(QKeySequence("Ctrl+Backspace"), this);
    m_RippleDeleteShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    m_ToggleStateShortcut = new QShortcut(QKeySequence(Qt::Key_D), this);
    m_ToggleStateShortcut->setContext(Qt::WidgetWithChildrenShortcut);

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
    // Sub-Components
    connect(m_TrackHeader, &STrackHeaderWidget::deleteSelectionRequested, this, &SequencerTimeline::DeleteSelected);

    // PlayerBridge
    connect(&_PlayerBridge, &PlayerBridge::playComponentUpdated, this, [this](const PlayerBuffer::PlayableComponent& component) -> void
    {
        if (component == PlayerBuffer::PlayableComponent::Sequence)
        {
            ViewerBuffer* viewer = _PlayerBridge.ActiveViewer();
            SetSequence(viewer->GetSequence());
        }
    });

    connect(m_Toolbar, &SToolbar::reset, this, &SequencerTimeline::Refresh);
    connect(m_Toolbar, &SToolbar::actionSwitched, this, [this](const SequencerAction& action) -> void { m_Context.SetAction(action); });

    // Controller
    connect(m_Context.Controller(), &SequencerController::editEffectRequested, this, &SequencerTimeline::editEffectRequested);

    connect(m_CutShortcut, &QShortcut::activated, this, &SequencerTimeline::Cut);
    connect(m_CopyShortcut, &QShortcut::activated, this, &SequencerTimeline::Copy);
    connect(m_PasteShortcut, &QShortcut::activated, this, [this]() -> void { Paste(QCursor::pos()); });
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

    // Menu
    connect(m_Menu, &SequencerContextMenu::createTrackRequested, this, [this]() -> void
    {
        m_Context.Controller()->CreateVideoTrack(m_Sequence);
    });
    connect(m_Menu, &SequencerContextMenu::cutSelectionRequested, this, &SequencerTimeline::Cut);
    connect(m_Menu, &SequencerContextMenu::copySelectionRequested, this, &SequencerTimeline::Copy);
    connect(m_Menu, &SequencerContextMenu::pasteRequested, this, &SequencerTimeline::Paste);
    connect(m_Menu, &SequencerContextMenu::deleteSelectionRequested, this, &SequencerTimeline::DeleteSelected);
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
    connect(m_Menu, &SequencerContextMenu::addEffectRequested, this, &SequencerTimeline::CreateEffect);
}

void SequencerTimeline::Connect(PlaybackSequence* sequence)
{
    connect(sequence, &PlaybackSequence::trackAdded, this, &SequencerTimeline::AddTrack);
    connect(sequence, &PlaybackSequence::trackAboutToBeRemoved, this, &SequencerTimeline::RemoveTrack);
    connect(sequence, &PlaybackSequence::maxTrackEffectsChanged, this, &SequencerTimeline::UpdateAll);
    connect(sequence, &PlaybackSequence::rangeChanged, m_Context.Controller(), &SequencerController::ResetRange);
}

void SequencerTimeline::Disconnect(PlaybackSequence* sequence)
{
    disconnect(sequence, &PlaybackSequence::trackAdded, this, &SequencerTimeline::AddTrack);
    disconnect(sequence, &PlaybackSequence::trackAboutToBeRemoved, this, &SequencerTimeline::RemoveTrack);
    disconnect(sequence, &PlaybackSequence::maxTrackEffectsChanged, this, &SequencerTimeline::UpdateAll);
    disconnect(sequence, &PlaybackSequence::rangeChanged, m_Context.Controller(), &SequencerController::ResetRange);
}

void SequencerTimeline::CreateEffect(const std::string& type)
{
    if (m_Context.SelectionModel()->HasTrackItemSelection())
        m_Context.Controller()->CreateEffect(m_Context.SelectionModel()->SelectedItems(), type);
    else if (m_Context.SelectionModel()->HasTrackSelection())
        m_Context.Controller()->CreateEffect(m_Context.SelectionModel()->SelectedTracks(), type);
}

void SequencerTimeline::DeleteSelected()
{
    if (m_Context.SelectionModel()->HasTrackSelection())
        m_Context.Controller()->RemoveTracks(m_Context.SelectionModel()->SelectedTracks());
    else if (m_Context.SelectionModel()->HasTrackItemSelection())
        m_Context.Controller()->RemoveTrackItems(m_Context.SelectionModel()->SelectedItems());
    else if (m_Context.SelectionModel()->HasEffectSelection())
        m_Context.Controller()->RemoveTimelineEffects(m_Context.SelectionModel()->SelectedEffects());

    m_Context.SelectionModel()->Clear();
}

void SequencerTimeline::RippleDeleteSelected()
{
    if (m_Context.SelectionModel()->HasTrackItemSelection())
        m_Context.Controller()->RippleRemoveTrackItems(m_Context.SelectionModel()->SelectedItems());

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

void SequencerTimeline::Cut()
{
    if (m_Context.SelectionModel()->HasTrackItemSelection())
        m_Context.Controller()->Cut(m_Context.SelectionModel()->SelectedItems());
}

void SequencerTimeline::Copy()
{
    if (m_Context.SelectionModel()->HasTrackItemSelection())
        m_Context.Controller()->Copy(m_Context.SelectionModel()->SelectedItems());
}

void SequencerTimeline::Paste(const QPoint& position)
{
    const QPointF mapped = m_View->mapToScene(m_View->mapFromGlobal(position));
    v_frame_t frame = m_Context.Geometry()->SceneXToFrame(mapped.x());
    if (STrack* track = m_Context.Controller()->TrackAt(mapped))
        m_Context.Controller()->Paste(Sequence::Context::Get(track->Track(), frame));
}

VOID_NAMESPACE_CLOSE
