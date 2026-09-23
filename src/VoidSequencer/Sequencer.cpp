// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QColorDialog>
#include <QCursor>
#include <QScrollBar>
#include <QStyle>
#include <QWheelEvent>

/* Internal */
#include "Sequencer.h"
#include "VoidCore/Logging.h"
#include "VoidDocker/DockPanel.h"
#include "VoidMediaPlayer/Player/PlayerBridge.h"
#include "VoidObjects/Sequence/Context.h"
#include "VoidSequencer/Graphics/STrack.h"
#include "VoidSequencer/Graphics/STrackItem.h"
#include "VoidSequencer/Graphics/STimelineScene.h"

VOID_NAMESPACE_OPEN

#define _FIT_PADDING 20

SequencerTimeline::SequencerTimeline(TimelineController* controller, QWidget* parent)
    : SequencerWidget(controller, parent)
{
    Connect();
}

SequencerTimeline::~SequencerTimeline()
{
}

void SequencerTimeline::SetSequence(const SharedPlaybackSequence& sequence)
{
    if (m_Context.HasActiveSequence())
        Disconnect(m_Context.Sequence().get());

    m_View->ResetScroll();
    m_Context.SetSequence(sequence);
    Connect(sequence.get());
    m_Context.Geometry()->SetSequence(sequence);
    m_VersionSwitcher->ResetModel(sequence->Project()->DataModel());

    Refresh();
    ResetTabText();
}

void SequencerTimeline::ClearSequence()
{
    if (m_Context.HasActiveSequence())
        Disconnect(m_Context.Sequence().get());

    m_Context.SetSequence(nullptr);
    m_Context.Geometry()->ResetSequence();
    m_VersionSwitcher->ResetModel(nullptr);

    m_TrackHeader->Clear();
    m_View->Clear();
    m_View->AddPlayhead();

    ResetTabText();
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
    if (m_Context.HasActiveSequence()) m_Context.Controller()->RazorAt(m_Context.Sequence(), frame);
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

void SequencerTimeline::SetHorizontalScale(float factor)
{
    m_Context.Geometry()->SetPixelsPerFrame(factor);
    m_View->Refresh();
    m_Ruler->Update();
}

void SequencerTimeline::Refresh()
{
    m_TrackHeader->Clear();
    m_View->Clear();
    m_View->AddPlayhead();

    for (const SharedPlaybackTrack& track : m_Context.Sequence()->VideoTracks())
        AddTrack(track);
}

void SequencerTimeline::FitAll()
{
    if (SharedPlaybackSequence sequence = m_Context.Sequence())
    {
        v_frame_t start = sequence->StartFrame();
        v_frame_t end = sequence->EndFrame();

        // Only update the fit, if we're not fitting currently, if we're currently fitting then should be okay
        if ((end - start + 1) > m_View->VisibleRange().duration)
        {
            m_View->FocusOn(start, end + _FIT_PADDING);
            m_Ruler->Update();
        }
    }
}

void SequencerTimeline::FitSelected()
{
    const std::unordered_set<SharedTrackItem>& items = m_Context.SelectionModel()->SelectedItems();
    if (items.empty())
        return;

    std::vector<SharedTrackItem> vecitems(items.size());
    std::transform(
        items.begin(),
        items.end(),
        vecitems.begin(),
        [](const SharedTrackItem& item) -> SharedTrackItem { return item; }
    );
    std::sort(vecitems.begin(), vecitems.end(), [](const SharedTrackItem& _a, const SharedTrackItem& _b) -> bool
    {
        return _a->TimelineIn() < _b->TimelineIn();
    });

    PlaybackTrack* track = vecitems.front()->Track();
    m_View->FocusOn(
        vecitems.front()->TimelineIn() - _FIT_PADDING,
        vecitems.back()->TimelineOut() + _FIT_PADDING,
        m_Context.Geometry()->TrackRect(track->Index()).y()
    );
    m_Ruler->Update();
}

void SequencerTimeline::ResetFit()
{
    SetHorizontalScale((float)m_HZoomSlider->value() / 10);
    m_View->ResetScroll();
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
    // connect(m_FitShortcut, &QShortcut::activated, this, &SequencerTimeline::FitAll);
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
        m_Context.Controller()->CreateVideoTrack(m_Context.Sequence());
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
    connect(m_Menu, &SequencerContextMenu::versionChangeRequested, this, &SequencerTimeline::SwitchVersion);
    connect(m_Menu, &SequencerContextMenu::versionExtremesChangeRequested, this, &SequencerTimeline::SwitchVersionExtremes);
    connect(m_Menu, &SequencerContextMenu::versionInspectionRequested, this, &SequencerTimeline::InspectVersions);
    connect(m_Menu, &SequencerContextMenu::versionScanRequested, this, &SequencerTimeline::ScanVersions);
    connect(m_Menu, &SequencerContextMenu::inOutSetRequested, this, &SequencerTimeline::ResetInOut);
    connect(m_Menu, &SequencerContextMenu::razorRequested, this, &SequencerTimeline::Razor);

    /// Fit
    connect(m_Menu, &SequencerContextMenu::fitAllRequested, this, &SequencerTimeline::FitAll);
    connect(m_Menu, &SequencerContextMenu::fitSelectedRequested, this, &SequencerTimeline::FitSelected);
    connect(m_Menu, &SequencerContextMenu::resetFitRequested, this, &SequencerTimeline::ResetFit);
}

void SequencerTimeline::Connect(PlaybackSequence* sequence)
{
    connect(sequence, &PlaybackSequence::trackAdded, this, &SequencerTimeline::AddTrack);
    connect(sequence, &PlaybackSequence::trackAboutToBeRemoved, this, &SequencerTimeline::RemoveTrack);
    connect(sequence, &PlaybackSequence::maxTrackEffectsChanged, this, &SequencerTimeline::UpdateAll);
    connect(sequence, &PlaybackSequence::rangeChanged, m_Context.Controller(), &SequencerController::ResetRange);
    connect(sequence, &PlaybackSequence::nameChanged, this, &SequencerTimeline::ResetTabText);
    connect(sequence, &PlaybackSequence::cleared, this, &SequencerTimeline::Clear);
    connect(sequence->Project(), &Core::Project::sequenceAboutToBeRemoved, this, [this](const SharedPlaybackSequence& sequence) -> void
    {
        if (sequence.get() == m_Context.Sequence().get()) ClearSequence();
    });
}

void SequencerTimeline::Disconnect(PlaybackSequence* sequence)
{
    disconnect(sequence, &PlaybackSequence::trackAdded, this, &SequencerTimeline::AddTrack);
    disconnect(sequence, &PlaybackSequence::trackAboutToBeRemoved, this, &SequencerTimeline::RemoveTrack);
    disconnect(sequence, &PlaybackSequence::maxTrackEffectsChanged, this, &SequencerTimeline::UpdateAll);
    disconnect(sequence, &PlaybackSequence::rangeChanged, m_Context.Controller(), &SequencerController::ResetRange);
    disconnect(sequence, &PlaybackSequence::nameChanged, this, &SequencerTimeline::ResetTabText);
    disconnect(sequence, &PlaybackSequence::cleared, this, &SequencerTimeline::Clear);
    disconnect(sequence->Project(), &Core::Project::sequenceAboutToBeRemoved, this, nullptr);
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
    const SSelectionModel* sel = m_Context.SelectionModel();
    if (sel->HasTrackItemSelection())
        m_Context.Controller()->Cut(sel->SelectedItems());
    if (sel->HasTrackSelection())
        m_Context.Controller()->Cut(sel->SelectedTracks());
}

void SequencerTimeline::Copy()
{
    const SSelectionModel* sel = m_Context.SelectionModel();
    if (sel->HasTrackItemSelection())
        m_Context.Controller()->Copy(sel->SelectedItems());
    if (sel->HasTrackSelection())
        m_Context.Controller()->Copy(sel->SelectedTracks());
}

void SequencerTimeline::Paste(const QPoint& position)
{
    const QPointF mapped = m_View->mapToScene(m_View->mapFromGlobal(position));
    v_frame_t frame = m_Context.Geometry()->SceneXToFrame(mapped.x());
    if (STrack* track = m_Context.Controller()->TrackAt(mapped))
        m_Context.Controller()->Paste(Sequence::Context::Get(track->Track(), frame));
    else
        m_Context.Controller()->Paste(Sequence::Context::Get(m_Context.Sequence()));
}

void SequencerTimeline::SwitchVersion(bool up)
{
    const SSelectionModel* sel = m_Context.SelectionModel();
    if (sel->HasTrackItemSelection())
        m_Context.Controller()->SwitchVersion(sel->SelectedItems(), up);
}

void SequencerTimeline::SwitchVersionExtremes(bool max)
{
    const SSelectionModel* sel = m_Context.SelectionModel();
    if (sel->HasTrackItemSelection())
        m_Context.Controller()->SwitchVersionExtremes(sel->SelectedItems(), max);
}

void SequencerTimeline::InspectVersions()
{
    const SSelectionModel* sel = m_Context.SelectionModel();
    const std::unordered_set<SharedTrackItem>& items = sel->SelectedItems();

    if (items.size() == 1)
    {
        const SharedTrackItem& item = *items.begin();

        const STimelineScene* scene = m_View->TimelineScene();
        const STrack* track = scene->TrackAt(item->Track()->Index());
        const STrackItem* sitem = track->Item(item);

        QPoint pos = mapToGlobal(m_View->mapFromScene(sitem->scenePos()));
        m_VersionSwitcher->Exec(item, QPoint(pos.x() - 100, pos.y()));
    }
}

void SequencerTimeline::ScanVersions()
{
    const SSelectionModel* sel = m_Context.SelectionModel();
    if (sel->HasTrackItemSelection())
        m_Context.Controller()->ScanVersions(sel->SelectedItems());
}

void SequencerTimeline::ResetInOut(bool selection)
{
    if (selection)
        return m_Context.Controller()->ResetTimelineInOut(m_Context.SelectionModel()->SelectedItems());
    
    if (const SharedTrackItem& item = m_Context.Sequence()->GetTrackItem(m_Context.TimeController()->Frame()))
        m_Context.Controller()->ResetTimelineInOut(item);
}

void SequencerTimeline::Razor(bool sequence)
{
    const v_frame_t frame = m_Context.TimeController()->Frame();
    if (sequence)
        return m_Context.Controller()->RazorAt(m_Context.Sequence(), frame);

    if (const SharedTrackItem& item = m_Context.Sequence()->GetTrackItem(frame))
        m_Context.Controller()->RazorAt(item->Track(), frame);
}

VOID_NAMESPACE_CLOSE
