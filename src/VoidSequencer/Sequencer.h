// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_H
#define _SEQUENCER_H

/* Qt */
#include <QLayout>
#include <QShortcut>
#include <QSlider>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "Descriptors.h"
#include "SContext.h"
#include "SMenu.h"
#include "STimelineGeometry.h"
#include "STimelineView.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "Widgets/STimelineRuler.h"
#include "Widgets/STrackHeaderWidget.h"
#include "Widgets/SToolbar.h"
#include "VoidMenuSystem/MenuSystem.h"

VOID_NAMESPACE_OPEN

class VOID_API SequencerTimeline : public QWidget
{
    Q_OBJECT
public:
    explicit SequencerTimeline(TimelineController* controller, QWidget* parent = nullptr);
    virtual inline QSize sizeHint() const override { return QSize(640, 300); }

    void InitMenu(MenuSystem* menuSystem);

    void SetSequence(const SharedPlaybackSequence& sequence);
    SharedPlaybackSequence ActiveSequence() const { return m_Sequence; }

    void AddTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);
    void RazorAt(v_frame_t frame);
    void RazorAt(const SharedPlaybackSequence& sequence, v_frame_t frame);
    void RazorAt(const SharedPlaybackTrack& track, v_frame_t frame);
    void MergeCut(const SharedPlaybackTrack& track, v_frame_t frame);
    void TrimItemHead(const SharedTrackItem& item, int handle);
    void TrimItemTail(const SharedTrackItem& item, int handle);

    void Refresh();

    void SetHorizontalScale(float factor);

signals:
    void editEffectRequested(Effect*);

private:
    QHBoxLayout* m_Layout;
    QSlider* m_HZoomSlider;
    SToolbar* m_Toolbar;
    STimelineRuler* m_Ruler;
    STrackHeaderWidget* m_TrackHeader;
    STimelineView* m_View;
    SequencerContextMenu* m_Menu;

    QShortcut* m_FitShortcut;
    QShortcut* m_DeleteShortcut;
    QShortcut* m_RippleDeleteShortcut;
    QShortcut* m_ToggleStateShortcut;

    SharedPlaybackSequence m_Sequence;
    SequencerContext m_Context;

private: /* Methods */
    void Build();
    void Connect();
    void Connect(PlaybackSequence* sequence);
    void Disconnect(PlaybackSequence* sequence);
    void CreateEffect(const std::string& type);
    void DeleteSelected();
    void RippleDeleteSelected();
    void ToggleItemState();
    void UpdateAll();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_H
