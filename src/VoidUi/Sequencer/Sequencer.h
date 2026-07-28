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
#include "SContext.h"
#include "SDescriptors.h"
#include "SMenu.h"
#include "STimelineGeometry.h"
#include "STimelineView.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidUi/Sequencer/Widgets/STimelineRuler.h"
#include "VoidUi/Sequencer/Widgets/STrackHeaderWidget.h"
#include "VoidUi/Sequencer/Widgets/SToolbar.h"

VOID_NAMESPACE_OPEN

class VOID_API SequencerTimeline : public QWidget
{
    Q_OBJECT
public:
    explicit SequencerTimeline(QWidget* parent = nullptr);
    virtual inline QSize sizeHint() const override { return QSize(640, 300); }

    void SetSequence(const SharedPlaybackSequence& sequence);
    SharedPlaybackSequence ActiveSequence() const { return m_Sequence; }

    void SetFrame(v_frame_t frame);
    void Refresh();

    void AddTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);

    void SetHorizontalZoom(float factor);

signals:
    void frameChangeRequested(v_frame_t);

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

    SharedPlaybackSequence m_Sequence;
    SequencerContext m_Context;

private: /* Methods */
    void Build();
    void Connect();
    void DeleteSelected();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_H
